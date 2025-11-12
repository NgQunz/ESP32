#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 16     // Chân DATA nối vào GPIO4
#define DHTTYPE DHT22 // Loại cảm biến

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  Serial.println("DHT22 Sensor Reading...");
}

void loop()
{
  float h = dht.readHumidity();    // Độ ẩm (%)
  float t = dht.readTemperature(); // Nhiệt độ (°C)

  // Kiểm tra lỗi đọc
  if (isnan(h) || isnan(t))
  {
    Serial.println("❌ Lỗi đọc DHT22!");
  }
  else
  {
    Serial.print("Nhiệt độ: ");
    Serial.print(t, 1);
    Serial.print(" °C | Độ ẩm: ");
    Serial.print(h, 1);
    Serial.println(" %");
  }

  delay(2000); // đo mỗi 2 giây
}
