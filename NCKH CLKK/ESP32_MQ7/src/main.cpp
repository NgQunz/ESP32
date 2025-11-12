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
