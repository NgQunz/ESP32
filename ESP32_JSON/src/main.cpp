#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include "RTClib.h"
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

HardwareSerial dfSerial(1); // UART1 (RX=16, TX=17)
DFRobotDFPlayerMini dfPlayer;

// DS3231 RTC
RTC_DS1307 rtc;

// HC12
HardwareSerial hc12(2); // UART2 (RX2: 27, TX2: 26)

// WiFi Info
const char *ssid = "102 Mini";
const char *password = "khongchopass";

// Relay pins
#define RELAY4 23 // dao chieu
#define RELAY1 18 // so 1
#define RELAY2 19 // so 2
#define RELAY3 21 // so 3

// DHT22 sensor
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RESET_PIN 4

unsigned long lastDHTRead = 0;
const unsigned long dhtInterval = 2000;
unsigned long lastHelloTime = 0;    // Thời gian nhận lệnh "hello" cuối cùng
unsigned long helloTimeout = 60000; // 60s mặc định

bool gioThoangActive = false;
unsigned long gioThoangTimer = 0;
int gioThoangStep = 0;
unsigned long gioThoangInterval = 5000; // 5s mỗi nấc
String previousFanState = "tat_quat";   // Lưu trạng thái quạt trước khi vào gió thoảng
bool previousXoayState = false;

AsyncWebServer server(80);
bool ready = false;

void setup()
{
  Serial.begin(115200);
  dht.begin();

  IPAddress local_IP(192, 168, 1, 100); // Địa chỉ IP tĩnh
  IPAddress gateway(192, 168, 1, 1);    // Gateway
  IPAddress subnet(255, 255, 255, 0);   // Subnet mask
  IPAddress primaryDNS(8, 8, 8, 8);     // DNS chính (Google)
  IPAddress secondaryDNS(8, 8, 4, 4);   // DNS phụ (Google)

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("Cấu hình IP tĩnh thất bại");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Wifi connecting...");
  }
  Serial.println("\nWiFi connected! IP: ");
  Serial.println(WiFi.localIP());

  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);

  Wire.begin(32, 33); // SDA, SCL

  if (!rtc.begin())
  {
    Serial.println("Không tìm thấy DS3231");
    while (1)
      ;
  }

  if (!rtc.isrunning())
  {
    Serial.println("DS3231 mất nguồn, thiết lập lại thời gian!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  dfSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  if (!dfPlayer.begin(dfSerial))
  {
    Serial.println("Không tìm thấy DFPlayer Mini!");
  }
  else
  {
    Serial.println("DFPlayer Mini sẵn sàng.");
    dfPlayer.volume(30); // Âm lượng từ 0 - 30
  }

  hc12.begin(9600, SERIAL_8N1, 27, 26); // RX, TX

  server.on("/relay", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
              DynamicJsonDocument doc(1024);
              DeserializationError error = deserializeJson(doc, data);
              if (error) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
              }

              String cmd = doc["command"];
              Serial.println("Received: " + cmd);

              // Kiểm tra thời gian hết hạn của "hello"
              if (ready && (millis() - lastHelloTime > helloTimeout)) {
                ready = false;
                Serial.println("Hết hạn 15 phút, cần gửi lại 'hello'");
              }

              if (cmd == "hello") {
                ready = true;
                lastHelloTime = millis(); // Cập nhật thời gian nhận "hello"
                dfPlayer.play(1);
                request->send(200, "application/json", "{\"status\":\"ESP32 ready\"}");
                return;
              }

              if (!ready) {
                dfPlayer.play(12);
                request->send(403, "application/json", "{\"error\":\"Not initialized. Send 'hello' first.\"}");
                return;
              }

              if (cmd == "tat_quat") {
                digitalWrite(RELAY1, LOW);
                digitalWrite(RELAY2, LOW);
                digitalWrite(RELAY3, LOW);
                digitalWrite(RELAY4, LOW);
                dfPlayer.play(7);
              } else if (cmd == "nac_1") {
                digitalWrite(RELAY1, HIGH);
                digitalWrite(RELAY2, LOW);
                digitalWrite(RELAY3, LOW);
                dfPlayer.play(2);
              } else if (cmd == "phat_nhac") {
                dfPlayer.play(20);
              } else if (cmd == "tinh_nho") {
                dfPlayer.play(18);
              } else if (cmd == "bac_bling") {
                dfPlayer.play(19);
              } else if (cmd == "tat_nhac") {
                dfPlayer.play(21);
              }
              else if (cmd == "nac_2") {
                digitalWrite(RELAY2, HIGH);
                digitalWrite(RELAY1, LOW);
                digitalWrite(RELAY3, LOW);
                dfPlayer.play(3);
              } else if (cmd == "nac_3") {
                digitalWrite(RELAY3, HIGH);
                digitalWrite(RELAY1, LOW);
                digitalWrite(RELAY2, LOW);
                dfPlayer.play(4);
              } else if (cmd == "quat_xoay") {
                digitalWrite(RELAY4, HIGH);
                dfPlayer.play(5);
              } else if (cmd == "tat_xoay") {
                digitalWrite(RELAY4, LOW);
                dfPlayer.play(6);
              } else if (cmd == "bat_den_1" || cmd == "bat_den_2" || cmd == "bat_den_3") {
                hc12.println(cmd);
                if (cmd== "bat_den_1") {
                  dfPlayer.play(13);
                } else if (cmd == "bat_den_2") {
                  dfPlayer.play(14);
                } else if (cmd == "bat_den_3") {
                  dfPlayer.play(8);
                }
              } else if (cmd == "tat_den_1" || cmd == "tat_den_2" || cmd == "tat_den_3") {
                hc12.println(cmd);
                if (cmd == "tat_den_1") {
                  dfPlayer.play(16);
                } else if (cmd == "tat_den_2") {
                  dfPlayer.play(15);
                } else if (cmd == "tat_den_3") {
                  dfPlayer.play(9);
                }
              } else if (cmd == "gio_thoang") {
                if (!gioThoangActive) {
                  // Lưu trạng thái hiện tại
                  if (digitalRead(RELAY1)) previousFanState = "nac_1";
                  else if (digitalRead(RELAY2)) previousFanState = "nac_2";
                  else if (digitalRead(RELAY3)) previousFanState = "nac_3";
                  else previousFanState = "tat_quat";
                  // Lưu trạng thái xoay
                  previousXoayState = digitalRead(RELAY4);
                  gioThoangActive = true;
                  gioThoangTimer = millis();
                  gioThoangStep = 0;
                  digitalWrite(RELAY4, HIGH);
                  dfPlayer.play(10); // bài nhạc báo hiệu gió thoảng
                }
              } else if (cmd == "tat_gio_thoang") {
                gioThoangActive = false;
                dfPlayer.play(11); 
                // Phục hồi trạng thái quạt
                if (previousFanState == "nac_1") {
                  digitalWrite(RELAY1, HIGH); digitalWrite(RELAY2, LOW); digitalWrite(RELAY3, LOW);
                } else if (previousFanState == "nac_2") {
                  digitalWrite(RELAY2, HIGH); digitalWrite(RELAY1, LOW); digitalWrite(RELAY3, LOW);
                } else if (previousFanState == "nac_3") {
                  digitalWrite(RELAY3, HIGH); digitalWrite(RELAY1, LOW); digitalWrite(RELAY2, LOW);
                } else {
                  digitalWrite(RELAY1, LOW); digitalWrite(RELAY2, LOW); digitalWrite(RELAY3, LOW);
                }
                // Khôi phục trạng thái xoay
                digitalWrite(RELAY4, previousXoayState ? HIGH : LOW);
              } else if (cmd.startsWith("time_out_")) {
                String timeStr = cmd.substring(9); // Cắt phần sau "time_out_"
                int multiplier = 1000; // default: seconds
                unsigned long value = 0;

                if (timeStr.endsWith("s")) {
                  value = timeStr.substring(0, timeStr.length() - 1).toInt();
                  multiplier = 1000;
                } else if (timeStr.endsWith("m")) {
                  value = timeStr.substring(0, timeStr.length() - 1).toInt();
                  multiplier = 60000;
                } else if (timeStr.endsWith("h")) {
                  value = timeStr.substring(0, timeStr.length() - 1).toInt();
                  multiplier = 3600000;
                } else {
                  request->send(400, "application/json", "{\"error\":\"Định dạng không hợp lệ. Dùng s, m hoặc h.\"}");
                  return;
                }

                if (value == 0) {
                  request->send(400, "application/json", "{\"error\":\"Giá trị timeout không hợp lệ.\"}");
                  return;
                }

                helloTimeout = value * multiplier;
                Serial.printf("Đã cập nhật helloTimeout = %lu ms\n", helloTimeout);
                dfPlayer.play(17); 
                request->send(200, "application/json", "{\"status\":\"Timeout updated\"}");
                return;
              } else {
                request->send(400, "application/json", "{\"error\":\"Unknown command\"}");
                return;
              }
              request->send(200, "application/json", "{\"status\":\"OK\"}"); });

  server.begin();
}

void loop()
{
  if (digitalRead(RESET_PIN) == LOW)
  {
    Serial.println("Đã nhấn nút reset!");
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    dfPlayer.stop();
    ready = false;
    delay(1000);
    while (digitalRead(RESET_PIN) == LOW)
      delay(10);
  }

  if (millis() - lastDHTRead >= dhtInterval)
  {
    lastDHTRead = millis();
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    DateTime now = rtc.now();

    Serial.print("Thời gian: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.println(now.second());

    if (isnan(temp) || isnan(hum))
    {
      Serial.println("Lỗi đọc DHT22!");
    }
    else
    {
      Serial.print("Nhiệt độ: ");
      Serial.print(temp);
      Serial.print("°C  |  Độ ẩm: ");
      Serial.print(hum);
      Serial.println("%");
    }
  }
  if (gioThoangActive)
  {
    if (millis() - gioThoangTimer >= gioThoangInterval)
    {
      gioThoangTimer = millis();
      gioThoangStep++;

      // Bật xoay nếu chưa bật
      digitalWrite(RELAY4, HIGH);

      // Điều khiển tốc độ theo chu kỳ
      int pattern[] = {1, 2, 3, 2};
      int speed = pattern[gioThoangStep % 4];

      if (speed == 1)
      {
        digitalWrite(RELAY1, HIGH);
        digitalWrite(RELAY2, LOW);
        digitalWrite(RELAY3, LOW);
      }
      else if (speed == 2)
      {
        digitalWrite(RELAY1, LOW);
        digitalWrite(RELAY2, HIGH);
        digitalWrite(RELAY3, LOW);
      }
      else if (speed == 3)
      {
        digitalWrite(RELAY1, LOW);
        digitalWrite(RELAY2, LOW);
        digitalWrite(RELAY3, HIGH);
      }
    }
  }
}