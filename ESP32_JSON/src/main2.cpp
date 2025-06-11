// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include <ArduinoJson.h>
// #include <DHT.h>
// #include <Wire.h>
// #include "RTClib.h"
// #include "DFRobotDFPlayerMini.h"
// #include <HardwareSerial.h>

// // DS3231 RTC
// RTC_DS1307 rtc;

// // DFPlayer
// HardwareSerial df_player(1);
// HardwareSerial hc12(2); // UART2 (RX2: 27, TX2: 26)
// DFRobotDFPlayerMini dfplayer;

// // Thông tin WiFi
// const char *ssid = "102 Mini";
// const char *password = "khongchopass";

// // Khai báo chân relay

// #define RELAY1 14
// #define RELAY2 18
// #define RELAY3 19
// #define RELAY4 21

// // Cảm biến DHT22
// #define DHTPIN 15 // Chân kết nối DHT22
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// #define RESET_PIN 4 // D4

// unsigned long lastDHTRead = 0;
// const unsigned long dhtInterval = 2000;

// AsyncWebServer server(80);
// bool ready = false;

// void setup()
// {
//   Serial.begin(115200);

//   // Khởi động DHT
//   dht.begin();

//   // Kết nối WiFi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.println("Wifi connecting...");
//   }
//   Serial.println("\nWiFi connected! IP: ");
//   Serial.println(WiFi.localIP());

//   pinMode(RESET_PIN, INPUT_PULLUP);

//   // Khởi tạo các chân relay
//   pinMode(RELAY1, OUTPUT);
//   pinMode(RELAY2, OUTPUT);
//   pinMode(RELAY3, OUTPUT);
//   pinMode(RELAY4, OUTPUT);

//   // Tắt tất cả relay ban đầu
//   digitalWrite(RELAY1, LOW);

//   digitalWrite(RELAY2, LOW);
//   digitalWrite(RELAY3, LOW);
//   digitalWrite(RELAY4, LOW);

//   Wire.begin(32, 33); // SDA = GPIO32, SCL = GPIO33

//   // DS3231 RTC init
//   if (!rtc.begin())
//   {
//     Serial.println("Không tìm thấy DS3231");
//     while (1)
//       ;
//   }
//   if (rtc.isrunning())
//   {
//     Serial.println("DS3231 mất nguồn, thiết lập lại thời gian!");
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // thiết lập thời gian bằng thời điểm biên dịch
//   }

//   hc12.begin(9600, SERIAL_8N1, 27, 26); // RX, TX

//   // DFPlayer init
//   df_player.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
//   if (!dfplayer.begin(df_player))
//   {
//     Serial.println("Không tìm thấy DFPlayer Mini");
//     while (1)
//       ;
//   }
//   dfplayer.volume(20); // Âm lượng từ 0-30

//   // Thiết lập webserver
//   server.on("/relay", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
//             {
//               DynamicJsonDocument doc(1024);
//               DeserializationError error = deserializeJson(doc, data);
//               if (error) {
//                 request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
//                 return;
//               }

//               String cmd = doc["command"];
//               Serial.println("Received: " + cmd);

//               if (cmd == "hello") {
//                 ready = true;
//                 request->send(200, "application/json", "{\"status\":\"ESP32 ready\"}");
//                 dfplayer.play(0);
//                 return;
//               }

//               if (!ready) {
//                 request->send(403, "application/json", "{\"error\":\"Not initialized. Send 'hello' first.\"}");
//                 return;
//               }

//               // T1: Tắt tất cả relay còn lại
//               if (cmd == "tat_quat") {
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY2, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 digitalWrite(RELAY4, LOW);
//                 dfplayer.play(5);
//               }
//               // T2: Bật T1, tắt T2 + T3
//               else if (cmd == "nac_1") {
//                 digitalWrite(RELAY1, HIGH);
//                 digitalWrite(RELAY2, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 dfplayer.play(1);
//               }
//               // T3: Bật T2, tắt T3 + T1
//               else if (cmd == "nac_2") {
//                 digitalWrite(RELAY2, HIGH);
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 dfplayer.play(2);
//               }
//               // T4: Bật T3, tắt T2 + T1
//               else if (cmd == "nac_3") {
//                 digitalWrite(RELAY3, HIGH);
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY2, LOW);
//                 dfplayer.play(3);
//               }
//               // T5: Toggle relay 5
//               else if (cmd == "quat_xoay") {

//                 digitalWrite(RELAY4,HIGH);
//                 dfplayer.play(4);
//               }
//               else if (cmd == "tat_xoay") {
//                 int current = digitalRead(RELAY4);
//                 digitalWrite(RELAY4,  LOW) ;
//                 dfplayer.play(4);
//               }
//               else if (cmd == "bat_den_1"|| cmd == "bat_den_2"|| cmd == "bat_den_3") {

//                 // Gửi lệnh qua HC12
//                 hc12.println(cmd); // Gửi chuỗi lệnh sang Nano qua HC12
//                 if (cmd == "bat_den_1")
//                 {
//                   dfplayer.play(6);
//                 }
//                 else if (cmd == "bat_den_2")
//                 {
//                   dfplayer.play(7);
//                 }
//                 else if (cmd == "bat_den_3")
//                 {
//                   dfplayer.play(8);
//                 }
//               }
//               else {
//                 request->send(400, "application/json", "{\"error\":\"Unknown command\"}");
//                 return;
//               }
//               request->send(200, "application/json", "{\"status\":\"OK\"}"); });
//   server.begin();
// }

// void loop()
// {
//   if (digitalRead(RESET_PIN) == LOW)
//   {
//     Serial.println("Đã nhấn nút reset!");

//     // Tắt tất cả relay
//     digitalWrite(RELAY1, LOW);
//     digitalWrite(RELAY2, LOW);
//     digitalWrite(RELAY3, LOW);
//     digitalWrite(RELAY4, LOW);

//     // Reset trạng thái sẵn sàng
//     ready = false;

//     delay(1000); // Chống dội nút

//     // Đợi thả nút
//     while (digitalRead(RESET_PIN) == LOW)
//       delay(10);
//   }
//   // Đọc cảm biến DHT22 mỗi 2 giây
//   if (millis() - lastDHTRead >= dhtInterval)
//   {
//     lastDHTRead = millis();
//     float temp = dht.readTemperature();
//     float hum = dht.readHumidity();

//     DateTime now = rtc.now();
//     Serial.print("Thời gian: ");
//     Serial.print(now.hour());
//     Serial.print(":");
//     Serial.print(now.minute());
//     Serial.print(":");
//     Serial.println(now.second());

//     if (isnan(temp) || isnan(hum))
//     {
//       Serial.println("Lỗi đọc DHT22!");
//     }
//     else
//     {
//       Serial.print("Nhiệt độ: ");
//       Serial.print(temp);
//       Serial.print("°C  |  Độ ẩm: ");
//       Serial.print(hum);
//       Serial.println("%");
//     }
//   }
// }
