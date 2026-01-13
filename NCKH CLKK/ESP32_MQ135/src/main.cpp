// #include <Arduino.h>

// const int MQ_PIN = 34;     // Chân ADC đọc MQ-135
// const float RL = 10000.0;  // Điện trở tải (10kΩ)
// const float VCC = 5.0;     // Điện áp nuôi cảm biến
// const float ADC_REF = 3.3; // Điện áp tham chiếu ADC ESP32
// const int ADC_MAX = 4095;  // 12-bit ADC
// float R0 = 10000.0;        // Giá trị R0 (hiệu chuẩn trong không khí sạch)

// // ==== Đường cong theo datasheet (log-log) ====
// // log10(ppm) = (log10(Rs/R0) - b) / m
// // hoặc ppm = 10^((log10(Rs/R0) - b) / m)
// const float M_NH3 = -0.48; // Hệ số slope NH3 (xấp xỉ)
// const float B_NH3 = 0.35;  // intercept NH3

// int samples = 10; // lọc trung bình

// float readVoltage()
// {
//   uint32_t sum = 0;
//   for (int i = 0; i < samples; i++)
//   {
//     sum += analogRead(MQ_PIN);
//     delay(10);
//   }
//   float adc = (float)sum / samples;
//   return (adc / ADC_MAX) * ADC_REF;
// }

// float calcRs(float vout)
// {
//   if (vout <= 0.0001)
//     return 1e6;
//   return RL * (VCC - vout) / vout;
// }

// float RsRo_to_ppm(float rs_ro, float m, float b)
// {
//   if (rs_ro <= 0)
//     return -1;
//   float log_ppm = (log10(rs_ro) - b) / m;
//   return pow(10, log_ppm);
// }
// void MQ135_run()
// {
//   float vout = readVoltage();
//   float Rs = calcRs(vout);
//   float ratio = Rs / R0;

//   float nh3_ppm = RsRo_to_ppm(ratio, M_NH3, B_NH3);

//   Serial.print("Vout: ");
//   Serial.print(vout, 3);
//   Serial.print(" V | Rs/R0: ");
//   Serial.print(ratio, 3);
//   Serial.print(" | NH3: ");
//   Serial.print(nh3_ppm, 1);
//   Serial.println(" ppm");
// }
// void setup()
// {
//   Serial.begin(9600);
//   analogSetPinAttenuation(MQ_PIN, ADC_11db);
//   pinMode(MQ_PIN, INPUT);

//   Serial.println("MQ-135 NH3 measurement");
//   delay(2000);
// }

// void loop()
// {
//   MQ135_run();
//   delay(2000);
// }
#include <Arduino.h>

#define MQ7_PIN 32         // Chân ADC ESP32 (chân 34 là chỉ đọc)
#define RL_VALUE 10000     // Điện trở tải 10kΩ
#define R0_CLEAN_AIR 10000 // R0 đo được trong không khí sạch (hiệu chuẩn trước)

float getMQ7Resistance(int adcValue)
{
  float sensorVoltage = ((float)adcValue / 4095.0) * 3.3; // ADC 12-bit, 3.3V
  float rs = (3.3 - sensorVoltage) * RL_VALUE / sensorVoltage;
  return rs;
}

float getCOppm(float rs)
{
  // Theo đặc tính MQ7 (log-log): ppm = 10^((log(Rs/R0) - b) / m)
  // Hệ số m và b lấy từ datasheet (xấp xỉ): m = -0.77, b = 1.699
  float ratio = rs / R0_CLEAN_AIR;
  float m = -0.77;
  float b = 1.699;
  float log_ppm = (log10(ratio) - b) / m;
  return pow(10, log_ppm);
}

void setup()
{
  Serial.begin(9600);
  delay(2000);
  Serial.println("MQ7 CO Sensor Start...");
}

void loop()
{
  int adcValue = analogRead(MQ7_PIN);
  float rs = getMQ7Resistance(adcValue);
  float co_ppm = getCOppm(rs);

  Serial.print("ADC = ");
  Serial.print(adcValue);
  Serial.print(" | Rs = ");
  Serial.print(rs);
  Serial.print(" | CO = ");
  Serial.print(co_ppm);
  Serial.println(" ppm");

  delay(2000);
}
