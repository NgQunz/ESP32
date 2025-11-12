#include <Wire.h>
#include <math.h>
#include "MAX30105.h"
#include "heartRate.h"

#define MAX30102_ADDRESS 0x57
#define BUFFER_SIZE 100 // ~1 giây dữ liệu cho SpO₂

MAX30105 particleSensor;

// ================== I2C LOW LEVEL ==================
void writeRegister(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(MAX30102_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readFIFO(uint32_t *red, uint32_t *ir)
{
  Wire.beginTransmission(MAX30102_ADDRESS);
  Wire.write(0x07); // FIFO_DATA_REGISTER
  Wire.endTransmission(false);
  Wire.requestFrom(MAX30102_ADDRESS, (uint8_t)6);

  uint32_t raw_red = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();
  uint32_t raw_ir = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();

  *red = raw_red & 0x3FFFF; // 18-bit
  *ir = raw_ir & 0x3FFFF;
}

// ================== MAX30102 CONFIG ==================
void setupMAX30102()
{
  writeRegister(0x09, 0x40); // reset
  delay(100);

  writeRegister(0x02, 0x00); // tắt ngắt
  writeRegister(0x03, 0x00);

  writeRegister(0x08, 0x4F); // FIFO config: average 4, rollover
  writeRegister(0x09, 0x03); // SpO2 mode (RED + IR)
  writeRegister(0x0A, 0x27); // SpO2 config: 18-bit, 100sps
  writeRegister(0x0C, 0x24); // LED RED
  writeRegister(0x0D, 0x24); // LED IR
}

// ================== SPO2 MATH ==================
float mean(float *buf, int N)
{
  float s = 0;
  for (int i = 0; i < N; i++)
    s += buf[i];
  return s / N;
}

float rms(float *buf, int N, float meanVal)
{
  float s = 0;
  for (int i = 0; i < N; i++)
  {
    float v = buf[i] - meanVal;
    s += v * v;
  }
  return sqrt(s / N);
}

float computeSpO2(float *red, float *ir, int N)
{
  float dc_red = mean(red, N);
  float dc_ir = mean(ir, N);
  float ac_red = rms(red, N, dc_red);
  float ac_ir = rms(ir, N, dc_ir);

  if (dc_red <= 0 || dc_ir <= 0 || ac_ir == 0)
    return -1;

  float R = (ac_red / dc_red) / (ac_ir / dc_ir);
  float SpO2 = 104 - 17 * R; // công thức tham khảo
  if (SpO2 < 50 || SpO2 > 100)
    return -1;
  return SpO2;
}

// ================== BPM ==================
#define AVG_BEAT_WINDOW 5 // gom 5 nhịp rồi mới in

float beatsPerMinute = -1;
int beatAvg = -1;
long lastBeat = 0;

float bpmBuffer[AVG_BEAT_WINDOW];
int bpmCount = 0;

// ================== GLOBALS ==================
float redBuf[BUFFER_SIZE];
float irBuf[BUFFER_SIZE];
int idx = 0;

// ================== MAIN ==================
void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);

  setupMAX30102();

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("Không tìm thấy MAX3010x. Kiểm tra wiring/power!");
    while (1)
      ;
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  Serial.println("Khởi động xong. Đặt ngón tay lên cảm biến...");
}

void loop()
{
  uint32_t red, ir;
  readFIFO(&red, &ir);

  // Lưu buffer cho SpO₂
  redBuf[idx] = (float)red;
  irBuf[idx] = (float)ir;
  idx++;
  if (idx >= BUFFER_SIZE)
    idx = 0;

  bool fingerDetected = (ir > 10000);

  // ========== TÍNH SpO₂ ==========
  float spo2 = -1;
  static float lastSpo2 = -1; // giá trị SpO2 lần trước

  if (fingerDetected && idx == 0)
  { // mỗi khi đủ 100 mẫu
    spo2 = computeSpO2(redBuf, irBuf, BUFFER_SIZE);
    if (spo2 > 0)
    {
      lastSpo2 = spo2; // cập nhật kết quả mới
      Serial.print("SpO2=");
      Serial.print(lastSpo2);
      Serial.print("%");
    }
  }

  // ========== TÍNH BPM ==========
  long irValue = particleSensor.getIR();
  static int lastBeatAvg = -1;

  if (fingerDetected && checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      bpmBuffer[bpmCount % AVG_BEAT_WINDOW] = beatsPerMinute;
      bpmCount++;
    }
  }

  if (bpmCount >= AVG_BEAT_WINDOW)
  {
    float sum = 0;
    for (int i = 0; i < AVG_BEAT_WINDOW; i++)
      sum += bpmBuffer[i];
    beatAvg = sum / AVG_BEAT_WINDOW;
    bpmCount = 0;
    lastBeatAvg = beatAvg; // cập nhật kết quả mới
    Serial.print(" | BPM=");
    Serial.println(lastBeatAvg);
  }
}