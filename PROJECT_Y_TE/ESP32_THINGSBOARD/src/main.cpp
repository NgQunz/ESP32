#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

#define MAX30205_ADDR 0x48 // A0=A1=A2=GND

// ----- WiFi -----
const char *ssid = "Ngocute";
const char *password = "00000000";

// ----- ThingsBoard MQTT -----
const char *mqtt_server = "demo.thingsboard.io";
const int mqtt_port = 1883;
const char *access_token = "DUwOgekvu6f32bA1gqFY"; // Access Token của device

WiFiClient espClient;
PubSubClient client(espClient);

// Hàm đọc nhiệt độ từ MAX30205
float readTemperature()
{
  Wire.beginTransmission(MAX30205_ADDR);
  Wire.write(0x00);            // Thanh ghi nhiệt độ
  Wire.endTransmission(false); // giữ bus (repeated start)

  Wire.requestFrom(MAX30205_ADDR, 2);
  if (Wire.available() == 2)
  {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t tempRaw = (msb << 8) | lsb;
    return tempRaw / 256.0;
  }
  return NAN;
}

// Kết nối lại MQTT
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Đang kết nối MQTT...");
    if (client.connect("ESP32_Client", access_token, NULL))
    {
      Serial.println("Thành công");
    }
    else
    {
      Serial.print("Lỗi, rc=");
      Serial.print(client.state());
      Serial.println(" -> Thử lại sau 5 giây");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA=21, SCL=22 cho ESP32

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" -> OK!");

  // Cấu hình MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  float temperature = readTemperature();
  if (isnan(temperature))
  {
    Serial.println("Lỗi đọc nhiệt độ, thử lại sau 1s...");
    delay(1000);
    temperature = readTemperature();
  }

  if (!isnan(temperature))
  {
    int systolic = random(110, 130);
    int diastolic = random(70, 90);
    int heartRate = random(60, 100);
    int spo2 = random(95, 100);

    Serial.printf("Temp: %.2f°C | Sys: %d | Dia: %d | HR: %d | SpO2: %d%%\n",
                  temperature, systolic, diastolic, heartRate, spo2);

    char payload[200];
    snprintf(payload, sizeof(payload),
             "{\"temperature\":%.2f,\"systolic\":%d,\"diastolic\":%d,\"heartRate\":%d,\"spo2\":%d}",
             temperature, systolic, diastolic, heartRate, spo2);

    client.publish("v1/devices/me/telemetry", payload);
    Serial.println("Đã gửi lên ThingsBoard: " + String(payload));
  }
  else
  {
    Serial.println("Không đọc được nhiệt độ sau lần thử lại, bỏ qua lần gửi này.");
  }

  delay(5000);
}
