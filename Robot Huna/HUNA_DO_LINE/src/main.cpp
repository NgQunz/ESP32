#include <Arduino.h>
#include <SoftPWM.h>

/* ================== pinout (MRTDuino) ==================
 * ML2/MR2 là DIR:
 *  - Port 16 -> Arduino pin 1 (TxD1) = ML2 (DIR trái)
 *  - Port 15 -> Arduino pin 0 (RxD1) = MR2 (DIR phải)
 * Lưu ý: dùng 0/1 có thể xung đột Serial.
 */
#define ML_DIR 1
#define MR_DIR 0

// PWM bằng SoftPWM (theo tài liệu MRTDuino)
#define ML_PWM 8
#define MR_PWM 6

/* ================== TÙY CHỌN CỰC TÍNH ==================
 * Nếu Speed(100,100) mà xe lùi → đảo giá trị này.
 */
const bool FORWARD_IS_HIGH = false; // true: HIGH=tiến, false: LOW=tiến
inline uint8_t DIR_FWD() { return FORWARD_IS_HIGH ? HIGH : LOW; }
inline uint8_t DIR_REV() { return FORWARD_IS_HIGH ? LOW : HIGH; }

/* ================== GIỚI HẠN & TRIM ================== */
const int PWM_CAP = 140;
inline int capPWM(int v) { return constrain(v, 0, PWM_CAP); }

// Cân hai bánh (tuỳ cơ khí thực tế)
float GAIN_L = 0.90f, GAIN_R = 0.95f;
int BIAS_L = 0, BIAS_R = 0;

/* ================== HIỆU CHUẨN VẬN TỐC ==================
 * CM_PER_SEC_BASE: vận tốc (cm/s) đo được ở PWM = SPEED_CAL_PWM.
 * Ví dụ đo được ~23 cm/s tại PWM=60 → CM_PER_SEC_BASE = 23.0f.
 */
float CM_PER_SEC_BASE = 32.0f;
const int SPEED_CAL_PWM = 60;

/* ================== BÙ TUYẾN TÍNH (cho quãng ngắn chuẩn) ==================
 * cm_eff = CM_SCALE * cm_req + CM_OFFSET
 * Gợi ý hiệu chỉnh nhanh (cùng một tốc độ Speed):
 *  - Gọi Distance(10) → đo d1 8 (cm thực), Distance(30) → đo d2 38
 *  - a = (d2 - d1) / (30 - 10);  b = d2 - a*30
 *  - CM_SCALE = 1/a;  CM_OFFSET = -b/a
 */
float CM_SCALE = 1.0f;
float CM_OFFSET = 4.5f;

/* ================== TRẠNG THÁI ================== */
// tốc độ mục tiêu “chạy mãi” (âm=lùi, dương=tiến)
static int g_targetL = 0, g_targetR = 0;
static bool g_dirty = true;

/* ================== HẠ TẦNG ĐỘNG CƠ ================== */
static void setWheelSigned(int pwmSigned, uint8_t dirPin, uint8_t pwmCh)
{
  pwmSigned = constrain(pwmSigned, -255, 255);
  if (pwmSigned > 0)
  {
    digitalWrite(dirPin, DIR_FWD());
    SoftPWMSet(pwmCh, capPWM(pwmSigned));
  }
  else if (pwmSigned < 0)
  {
    digitalWrite(dirPin, DIR_REV());
    SoftPWMSet(pwmCh, capPWM(-pwmSigned));
  }
  else
  {
    SoftPWMSet(pwmCh, 0);
  }
}

static void applyTrimAndWrite(int leftSigned, int rightSigned)
{
  int sL = (leftSigned >= 0) ? +1 : -1;
  int sR = (rightSigned >= 0) ? +1 : -1;
  int mL = abs(leftSigned);
  int mR = abs(rightSigned);

  long l = lround(mL * GAIN_L) + BIAS_L;
  long r = lround(mR * GAIN_R) + BIAS_R;

  setWheelSigned(sL * capPWM((int)l), ML_DIR, ML_PWM);
  setWheelSigned(sR * capPWM((int)r), MR_DIR, MR_PWM);
}

/* ================== TÍNH THỜI GIAN THEO QUÃNG ĐƯỜNG ================== */
static unsigned long msForDistance_linear(float cm_req, int pwmL, int pwmR)
{
  // Bù tuyến tính để quãng ngắn cũng chuẩn
  float cm_eff = CM_SCALE * cm_req + CM_OFFSET;
  if (cm_eff <= 0)
    return 0;

  // dùng trung bình 2 bánh để ước lượng vận tốc tiến
  int pwmMag = (abs(pwmL) + abs(pwmR)) / 2;
  if (pwmMag <= 0)
    return 0;

  float v_cm_s = CM_PER_SEC_BASE * ((float)pwmMag / (float)SPEED_CAL_PWM); // cm/s
  if (v_cm_s < 0.001f)
    v_cm_s = 0.001f;

  float t_ms = (cm_eff / v_cm_s) * 1000.0f;
  if (t_ms < 0)
    t_ms = 0;
  return (unsigned long)(t_ms + 0.5f);
}

/* ================== API BẠN CẦN ================== */
// 1) Đặt tốc độ để xe “chạy mãi”
void Speed(int left, int right)
{
  g_targetL = constrain(left, -255, 255);
  g_targetR = constrain(right, -255, 255);
  g_dirty = true;
}

// 2) Đi đúng quãng đường theo tốc độ hiện tại rồi cắt PWM về 0 ngay
void Distance(float cm)
{
  if (cm <= 0)
    return;
  if (g_targetL == 0 && g_targetR == 0)
    return; // chưa đặt tốc độ thì bỏ

  // Áp tốc độ hiện tại ngay, không ramp, không phanh ngược
  applyTrimAndWrite(g_targetL, g_targetR);

  // Giữ đến hết thời gian cần thiết
  unsigned long hold = msForDistance_linear(cm, g_targetL, g_targetR);
  unsigned long t0 = millis();
  while (millis() - t0 < hold)
  {
    // giữ đúng tốc độ đang đặt; nếu bạn gọi Speed() giữa chừng, tốc độ sẽ cập nhật ngay
    applyTrimAndWrite(g_targetL, g_targetR);
    delay(2);
  }

  // Cắt PWM ngay (không phanh gắt)
  SoftPWMSet(ML_PWM, 0);
  SoftPWMSet(MR_PWM, 0);
}
void turn_Left_90()
{
  Speed(-100, 100);
  Distance(22);
  Speed(0, 0);
}
void turn_Right_90()
{
  Speed(100, -100);
  Distance(22);
  Speed(0, 0);
}
void runWithDS(int distance)
{
  Speed(60, 60);
  Distance(distance);
  Speed(0, 0);
}

/* ================== SETUP / LOOP ================== */
void setup()
{
  pinMode(ML_DIR, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_PWM, OUTPUT);

  SoftPWMBegin();
  SoftPWMSet(ML_PWM, 0);
  SoftPWMSet(MR_PWM, 0);

  // Nếu cần Serial, cân nhắc đổi chân DIR thay vì dùng 0/1
  // Serial.begin(115200);
}

void loop()
{
  // Áp “chạy mãi” nếu vừa thay đổi
  if (g_dirty)
  {
    applyTrimAndWrite(g_targetL, g_targetR);
    g_dirty = false;
  }
  runWithDS(37);
  delay(200);
  turn_Right_90();
  delay(200);
  runWithDS(25);
  delay(200);
  turn_Right_90();
  delay(200);
  runWithDS(30);
  delay(200);
  turn_Left_90();
  delay(200);
  runWithDS(45);
  delay(200);
  turn_Left_90();
  delay(200);
  runWithDS(75);
  delay(200);
  turn_Left_90();
  delay(200);
  runWithDS(43);
  delay(200);
  turn_Left_90();
  delay(200);
  runWithDS(20);
  delay(200);
  turn_Right_90();
  delay(200);
  runWithDS(35);
  delay(200);
  turn_Right_90();
  delay(200);
  runWithDS(30);
  delay(200);

  Speed(0, 0);
  while (1)
    ;
  // Speed(-100,100);
  // Distance(27);
  // Speed(0,0);
  // while(1);

  // ======= Ví dụ dùng đúng style bạn muốn (bật/tắt tuỳ kiểm thử) =======
  /*
  // 1) Chạy mãi:
  Speed(75, 75);

  // 2) Đi đúng 30 cm theo tốc độ đang đặt, sau đó dừng:
  Distance(30);
  delay(1500);

  // 3) Thử quãng ngắn 10 cm (để bạn hiệu chỉnh CM_SCALE/CM_OFFSET):
  Speed(75, 75);
  Distance(10);
  delay(2000);

  // 4) Dừng hẳn (ngừng chạy mãi):
  Speed(0, 0);
  */
}
