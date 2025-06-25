// #define HTTP_PARSER_H_
// #include <Arduino.h>
// #include <WiFiManager.h>
// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include <ArduinoJson.h>
// #include <DHT.h>
// #include <Wire.h>
// #include "RTClib.h"
// #include <HardwareSerial.h>
// #include <DFRobotDFPlayerMini.h>

// HardwareSerial dfSerial(1); // UART1 (RX=16, TX=17)
// DFRobotDFPlayerMini dfPlayer;

// // DS1307 RTC
// RTC_DS1307 rtc;

// // HC12
// HardwareSerial hc12(2); // UART2 (RX2: 27, TX2: 26)

// // WiFi Info
// // const char *ssid = "P603";
// // const char *password = "0904362626";
// const char *ssid = "Quan";          // Thay bằng tên WiFi mới
// const char *password = "02112004@"; // Thay bằng mật khẩu WiFi
// // Relay pins
// #define RELAY4 23 // dao chieu
// #define RELAY1 18 // so 1
// #define RELAY2 19 // so 2
// #define RELAY3 21 // so 3

// // DHT22 sensor
// #define DHTPIN 15
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// #define RESET_PIN 4

// unsigned long lastDHTRead = 0;
// const unsigned long dhtInterval = 2000;
// unsigned long lastHelloTime = 0;     // Thời gian nhận lệnh "hello" cuối cùng
// unsigned long helloTimeout = 900000; // 60s mặc định

// bool gioThoangActive = false;
// unsigned long gioThoangTimer = 0;
// int gioThoangStep = 0;
// unsigned long gioThoangInterval = 5000; // 5s mỗi nấc
// String previousFanState = "tat_quat";   // Lưu trạng thái quạt trước khi vào gió thoảng
// bool previousXoayState = false;

// AsyncWebServer server(80);
// bool ready = false;

// void setup()
// {
//   Serial.begin(115200);
//   dht.begin();

//   WiFiManager wm;

//   IPAddress local_IP(192, 168, 0, 90);
//   IPAddress gateway(192, 168, 0, 1);
//   IPAddress subnet(255, 255, 255, 0);

//   // Gán IP tĩnh trước khi bắt đầu
//   wm.setSTAStaticIPConfig(local_IP, gateway, subnet);

//   // Auto connect với SSID lưu sẵn, nếu không sẽ mở AP cấu hìnhaa
//   if (!wm.autoConnect("ESP32_Config", "12345678"))
//   {
//     Serial.println("Không kết nối được WiFi, khởi động lại...");
//     // wm.resetSettings();
//     delay(3000);
//     ESP.restart();
//   }

//   Serial.println("Đã kết nối WiFi!");
//   Serial.print("IP: ");
//   Serial.println(WiFi.localIP());

//   pinMode(RESET_PIN, INPUT_PULLUP);
//   pinMode(RELAY1, OUTPUT);
//   pinMode(RELAY2, OUTPUT);
//   pinMode(RELAY3, OUTPUT);
//   pinMode(RELAY4, OUTPUT);

//   digitalWrite(RELAY1, LOW);
//   digitalWrite(RELAY2, LOW);
//   digitalWrite(RELAY3, LOW);
//   digitalWrite(RELAY4, LOW);

//   Wire.begin(32, 33); // SDA, SCL

//   if (!rtc.begin())
//   {
//     Serial.println("Không tìm thấy DS1307");
//     while (1)
//       ;
//   }

//   if (!rtc.isrunning())
//   {
//     Serial.println("DS1307 mất nguồn, thiết lập lại thời gian!");
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//   }
//   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

//   dfSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
//   if (!dfPlayer.begin(dfSerial))
//   {
//     Serial.println("Không tìm thấy DFPlayer Mini!");
//   }
//   else
//   {
//     Serial.println("DFPlayer Mini sẵn sàng.");
//     dfPlayer.volume(30); // Âm lượng từ 0 - 30
//   }

//   hc12.begin(9600, SERIAL_8N1, 27, 26); // RX, TX

//   server.onNotFound([](AsyncWebServerRequest *request)
//                     {
//               String path = request->url().substring(1);  // Bỏ dấu '/'
//               Serial.println("GET command: " + path);

//               String cmd = path;

//               // Kiểm tra thời gian hết hạn của "hello"
//               if (ready && (millis() - lastHelloTime > helloTimeout)) {
//                 ready = false;
//                 Serial.println("Hết hạn 1 phút, cần gửi lại 'hello'");
//               }

//               if (cmd == "xin_chao") {
//                 ready = true;
//                 lastHelloTime = millis(); // Cập nhật thời gian nhận "hello"
//                 dfPlayer.play(1);
//                 request->send(200, "application/json", "{\"status\":\"ESP32 ready\"}");
//                 return;
//               }

//               if (!ready) {
//                 dfPlayer.play(12);
//                 request->send(403, "application/json", "{\"error\":\"Not initialized. Send 'hello' first.\"}");
//                 return;
//               }

//               if (cmd == "tat_quat") {
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY2, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 digitalWrite(RELAY4, LOW);
//                 dfPlayer.play(7);
//               } else if (cmd == "bat_quat_so_mot") {
//                 digitalWrite(RELAY1, HIGH);
//                 digitalWrite(RELAY2, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 dfPlayer.play(2);
//               } else if (cmd == "phat_nhac") {
//                 dfPlayer.play(20);
//               } else if (cmd == "tinh_nho") {
//                 dfPlayer.play(18);
//               } else if (cmd == "bac_ninh") {
//                 dfPlayer.play(19);
//               } else if (cmd == "tat_nhac") {
//                 dfPlayer.play(21);
//               }
//               else if (cmd == "bat_quat_so_hai") {
//                 digitalWrite(RELAY2, HIGH);
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY3, LOW);
//                 dfPlayer.play(3);
//               } else if (cmd == "bat_quat_so_ba") {
//                 digitalWrite(RELAY3, HIGH);
//                 digitalWrite(RELAY1, LOW);
//                 digitalWrite(RELAY2, LOW);
//                 dfPlayer.play(4);
//               } else if (cmd == "quat_xoay") {
//                 digitalWrite(RELAY4, HIGH);
//                 dfPlayer.play(5);
//               } else if (cmd == "tat_xoay") {
//                 digitalWrite(RELAY4, LOW);
//                 dfPlayer.play(6);
//               } else if (cmd == "bat_den_mot" || cmd == "bat_den_mot" || cmd == "bat_den_mot") {
//                 hc12.println(cmd);
//                 if (cmd== "bat_den_mot") {
//                   dfPlayer.play(13);
//                 } else if (cmd == "bat_den_hai") {
//                   dfPlayer.play(14);
//                 } else if (cmd == "bat_den_ba") {
//                   dfPlayer.play(8);
//                 }
//               } else if (cmd == "tat_den_mot" || cmd == "tat_den_hai" || cmd == "tat_den_ba") {
//                 hc12.println(cmd);
//                 if (cmd == "tat_den_mot") {
//                   dfPlayer.play(16);
//                 } else if (cmd == "tat_den_hai") {
//                   dfPlayer.play(15);
//                 } else if (cmd == "tat_den_ba") {
//                   dfPlayer.play(9);
//                 }
//               } else if (cmd == "gio_thoang") {
//                 if (!gioThoangActive) {
//                   // Lưu trạng thái hiện tại
//                   if (digitalRead(RELAY1)) previousFanState = "bat_quat_so_mot";
//                   else if (digitalRead(RELAY2)) previousFanState = "bat_quat_so_hai";
//                   else if (digitalRead(RELAY3)) previousFanState = "bat_quat_so_ba";
//                   else previousFanState = "tat_quat";
//                   // Lưu trạng thái xoay
//                   previousXoayState = digitalRead(RELAY4);
//                   gioThoangActive = true;
//                   gioThoangTimer = millis();
//                   gioThoangStep = 0;
//                   digitalWrite(RELAY4, HIGH);
//                   dfPlayer.play(10); // bài nhạc báo hiệu gió thoảng
//                 }
//               } else if (cmd == "tat_gio_thoang") {
//                 gioThoangActive = false;
//                 dfPlayer.play(11);
//                 // Phục hồi trạng thái quạt
//                 if (previousFanState == "bat_quat_so_mot") {
//                   digitalWrite(RELAY1, HIGH); digitalWrite(RELAY2, LOW); digitalWrite(RELAY3, LOW);
//                 } else if (previousFanState == "bat_quat_so_hai") {
//                   digitalWrite(RELAY2, HIGH); digitalWrite(RELAY1, LOW); digitalWrite(RELAY3, LOW);
//                 } else if (previousFanState == "bat_quat_so_ba") {
//                   digitalWrite(RELAY3, HIGH); digitalWrite(RELAY1, LOW); digitalWrite(RELAY2, LOW);
//                 } else {
//                   digitalWrite(RELAY1, LOW); digitalWrite(RELAY2, LOW); digitalWrite(RELAY3, LOW);
//                 }
//                 // Khôi phục trạng thái xoay
//                 digitalWrite(RELAY4, previousXoayState ? HIGH : LOW);
//               } else if (cmd.startsWith("time_out_")) {
//                 String timeStr = cmd.substring(9); // Cắt phần sau "time_out_"
//                 int multiplier = 1000; // default: seconds
//                 unsigned long value = 0;

//                 if (timeStr.endsWith("s")) {
//                   value = timeStr.substring(0, timeStr.length() - 1).toInt();
//                   multiplier = 1000;
//                 } else if (timeStr.endsWith("m")) {
//                   value = timeStr.substring(0, timeStr.length() - 1).toInt();
//                   multiplier = 60000;
//                 } else if (timeStr.endsWith("h")) {
//                   value = timeStr.substring(0, timeStr.length() - 1).toInt();
//                   multiplier = 3600000;
//                 } else {
//                   request->send(400, "application/json", "{\"error\":\"Định dạng không hợp lệ. Dùng s, m hoặc h.\"}");
//                   return;
//                 }

//                 if (value == 0) {
//                   request->send(400, "application/json", "{\"error\":\"Giá trị timeout không hợp lệ.\"}");
//                   return;
//                 }

//                 helloTimeout = value * multiplier;
//                 Serial.printf("Đã cập nhật helloTimeout = %lu ms\n", helloTimeout);
//                 dfPlayer.play(17);
//                 request->send(200, "application/json", "{\"status\":\"Timeout updated\"}");
//                 return;
//               } else {
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
//     digitalWrite(RELAY1, LOW);
//     digitalWrite(RELAY2, LOW);
//     digitalWrite(RELAY3, LOW);
//     digitalWrite(RELAY4, LOW);
//     dfPlayer.stop();
//     ready = false;
//     delay(1000);
//     while (digitalRead(RESET_PIN) == LOW)
//       delay(10);
//   }

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
//   if (gioThoangActive)
//   {
//     if (millis() - gioThoangTimer >= gioThoangInterval)
//     {
//       gioThoangTimer = millis();
//       gioThoangStep++;

//       // Bật xoay nếu chưa bật
//       digitalWrite(RELAY4, HIGH);

//       // Điều khiển tốc độ theo chu kỳ
//       int pattern[] = {1, 2, 3, 2};
//       int speed = pattern[gioThoangStep % 4];

//       if (speed == 1)
//       {
//         digitalWrite(RELAY1, HIGH);
//         digitalWrite(RELAY2, LOW);
//         digitalWrite(RELAY3, LOW);
//       }
//       else if (speed == 2)
//       {
//         digitalWrite(RELAY1, LOW);
//         digitalWrite(RELAY2, HIGH);
//         digitalWrite(RELAY3, LOW);
//       }
//       else if (speed == 3)
//       {
//         digitalWrite(RELAY1, LOW);
//         digitalWrite(RELAY2, LOW);
//         digitalWrite(RELAY3, HIGH);
//       }
//     }
//   }
// }