#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 32     // DHT11 sensor pin
#define DHTTYPE DHT11 // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

String readDHTTemperature() // đọc nhiệt độ và trả lại dưới dạng chuỗi
{
  float t = dht.readTemperature();
  if (isnan(t)) 
  {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else 
  {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() // đọc độ ẩm và trả lại dưới dạng chuỗi
{
  float h = dht.readHumidity();
  if (isnan(h)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else 
  {
    Serial.println(h);
    return String(h);
  }
}