#include <Arduino.h>

const int MQ_PIN = 34;     // Chân ADC đọc MQ-135
const float RL = 10000.0;  // Điện trở tải (10kΩ)
const float VCC = 5.0;     // Điện áp nuôi cảm biến
const float ADC_REF = 3.3; // Điện áp tham chiếu ADC ESP32
const int ADC_MAX = 4095;  // 12-bit ADC
float R0 = 10000.0;        // Giá trị R0 (hiệu chuẩn trong không khí sạch)

// ==== Đường cong theo datasheet (log-log) ====
// log10(ppm) = (log10(Rs/R0) - b) / m
// hoặc ppm = 10^((log10(Rs/R0) - b) / m)
const float M_NH3 = -0.48; // Hệ số slope NH3 (xấp xỉ)
const float B_NH3 = 0.35;  // intercept NH3

int samples = 10; // lọc trung bình

float readVoltage()
{
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++)
  {
    sum += analogRead(MQ_PIN);
    delay(10);
  }
  float adc = (float)sum / samples;
  return (adc / ADC_MAX) * ADC_REF;
}

float calcRs(float vout)
{
  if (vout <= 0.0001)
    return 1e6;
  return RL * (VCC - vout) / vout;
}

float RsRo_to_ppm(float rs_ro, float m, float b)
{
  if (rs_ro <= 0)
    return -1;
  float log_ppm = (log10(rs_ro) - b) / m;
  return pow(10, log_ppm);
}
void MQ135_run()
{
  float vout = readVoltage();
  float Rs = calcRs(vout);
  float ratio = Rs / R0;

  float nh3_ppm = RsRo_to_ppm(ratio, M_NH3, B_NH3);

  Serial.print("Vout: ");
  Serial.print(vout, 3);
  Serial.print(" V | Rs/R0: ");
  Serial.print(ratio, 3);
  Serial.print(" | NH3: ");
  Serial.print(nh3_ppm, 1);
  Serial.println(" ppm");
}
void setup()
{
  Serial.begin(9600);
  analogSetPinAttenuation(MQ_PIN, ADC_11db);
  pinMode(MQ_PIN, INPUT);

  Serial.println("MQ-135 NH3 measurement");
  delay(2000);
}

void loop()
{
  MQ135_run();
  delay(2000);
}
