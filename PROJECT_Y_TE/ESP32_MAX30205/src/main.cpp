#include <Wire.h>
#include <Arduino.h>
#define MAX30205_ADDR 0x48 // A0=A1=A2=GND

float readTemperature()
{
  // B1: Trỏ thanh ghi Temperature
  Wire.beginTransmission(MAX30205_ADDR);
  Wire.write(0x00);            // Thanh ghi nhiệt độ
  Wire.endTransmission(false); // giữ bus (repeated start)

  // B2: Đọc 2 byte dữ liệu
  Wire.requestFrom(MAX30205_ADDR, 2);
  if (Wire.available() == 2)
  {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    // Ghép thành số signed 16-bit
    int16_t tempRaw = (msb << 8) | lsb;

    // Chia cho 256 để ra °C
    return tempRaw / 256.0;
  }
  return NAN; // nếu đọc lỗi
}

void setup()
{
  Wire.begin(21, 22); // ESP32 classic: SDA=21, SCL=22
  Serial.begin(115200);
}

void loop()
{
  float tempC = readTemperature();
  if (!isnan(tempC))
  {
    Serial.print("Nhiet do: ");
    Serial.print(tempC, 4); // in 4 chữ số thập phân
    Serial.println(" *C");
  }
  else
  {
    Serial.println("Loi doc cam bien");
  }
  delay(1000);
}
