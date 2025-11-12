#include <Arduino.h>
#include <SoftPWM.h>

/* ================== PINOUT (MRTDuino) ================== */
#define ML_DIR 1
#define MR_DIR 0
#define ML_PWM 8
#define MR_PWM 6

/* ================== TÙY CHỌN CỰC TÍNH ================== */
const bool FORWARD_IS_HIGH = true; // true: HIGH=tiến, false: LOW=tiến
inline uint8_t DIR_FWD() { return FORWARD_IS_HIGH ? HIGH : LOW; }
inline uint8_t DIR_REV() { return FORWARD_IS_HIGH ? LOW : HIGH; }

/* ================== GIỚI HẠN & TRIM ================== */
const int PWM_CAP = 140;
inline int capPWM(int v) { return constrain(v, 0, PWM_CAP); }

float GAIN_L = 0.78f, GAIN_R = 1.18f;
int BIAS_L = 0, BIAS_R = 0;

/* ================== HIỆU CHUẨN VẬN TỐC ================== */
float CM_PER_SEC_BASE = 34.5f;
const int SPEED_CAL_PWM = 70;

/* ================== BÙ TUYẾN TÍNH & QUÃNG NGẮN ================== */
float CM_SCALE = 0.909f;
float CM_OFFSET = 4.545f;
float NEAR_OFFSET_CM = 0.0f;
float NEAR_APPLY_UPTO = 20.0f;

/* ================== TRẠNG THÁI ================== */
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

/* ================== PHANH CHỦ ĐỘNG (ACTIVE REVERSE TAP) ==================
 * Vì 1 DIR + 1 PWM không tạo được IN1=IN2=H ổn định, ta phanh bằng cách
 * bơm xung NGƯỢC nhẹ, rất ngắn rồi cắt hẳn.
 */
const int BRAKE_BACK_PWM = 60; // độ mạnh xung ngược (40..90 tuỳ quán tính)
const uint16_t BRAKE_MS = 120; // thời gian xung ngược (80..150 ms)

static void stopActiveBrake()
{
  // bơm xung ngược đều cả hai bánh
  applyTrimAndWrite(-BRAKE_BACK_PWM, -BRAKE_BACK_PWM);
  delay(BRAKE_MS);

  // cắt về 0 ngay
  SoftPWMSet(ML_PWM, 0);
  SoftPWMSet(MR_PWM, 0);
  // (không cần chỉnh DIR; để đó không sao)
}

/* ================== TÍNH THỜI GIAN THEO QUÃNG ĐƯỜNG ================== */
static unsigned long msForDistance_linear(float cm_req, int pwmL, int pwmR)
{
  float cm_eff = CM_SCALE * cm_req + CM_OFFSET;
  if (cm_req <= NEAR_APPLY_UPTO)
    cm_eff += NEAR_OFFSET_CM;
  if (cm_eff <= 0.0f)
    return 0;
  int pwmMag = (abs(pwmL) + abs(pwmR)) / 2;
  if (pwmMag <= 0)
    return 0;

  float v_cm_s = CM_PER_SEC_BASE * ((float)pwmMag / (float)SPEED_CAL_PWM);
  if (v_cm_s < 0.001f)
    v_cm_s = 0.001f;

  float t_ms = (cm_eff / v_cm_s) * 1000.0f;
  if (t_ms < 0)
    t_ms = 0;
  return (unsigned long)(t_ms + 0.5f);
}

/* ================== API ================== */
void Speed(int left, int right)
{
  g_targetL = constrain(left, -255, 255);
  g_targetR = constrain(right, -255, 255);
  g_dirty = true;
}

// Dùng micros() để giảm sai số do delay()
void Distance(float cm)
{
  if (cm <= 0)
    return;
  if (g_targetL == 0 && g_targetR == 0)
    return;

  applyTrimAndWrite(g_targetL, g_targetR);

  unsigned long hold_us = msForDistance_linear(cm, g_targetL, g_targetR) * 1000UL;
  unsigned long t0 = micros();
  while ((unsigned long)(micros() - t0) < hold_us)
  {
    // giữ tốc; tránh in Serial để không trễ
  }

  // Phanh chủ động để “đóng bến” dứt khoát
  stopActiveBrake();
}

/* ================== CÁC HÀM TIỆN ================== */
void turn_Left(int degree)
{
  Speed(-73, 70);
  Distance(32 + degree); // 32
  Speed(0, 0);
}
void turn_Right(int degree)
{
  Speed(70, -73);
  Distance(35 + degree); // 35
  Speed(0, 0);
}
void runWithDS(int distance)
{
  Speed(73, 73);
  Distance(distance * 1.2);
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

  // Serial.begin(115200); // tránh dùng vì 0/1 trùng DIR
}

void loop()
{
  if (g_dirty)
  {
    applyTrimAndWrite(g_targetL, g_targetR);
    g_dirty = false;
  }

  // test
  // Speed(70,73);
  // Distance(20);
  // delay(1000);
  delay(5000);
  runWithDS(59);
  delay(500);
  turn_Right(10);
  delay(500);
  runWithDS(35);
  delay(500);
  while (1)
    ;
}