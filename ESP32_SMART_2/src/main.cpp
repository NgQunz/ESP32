// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <DHT.h>

// // WiFi Info
// const char *ssid = "102 Mini";
// const char *password = "khongchopass";

// // Server URL
// String URL = "http://192.168.1.124/ESP32_SMART_2/testdata.php";

// // DHT22
// #define DHTPIN 23
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// // MQ2
// #define MQ2_PIN 34

// // Buzzer
// #define BUZZER_PIN 12

// // Sensor data
// float tempc = 0.0;
// int humi = 0;
// int ppm = 0;

// // Timing
// const unsigned long interval = 2000;
// unsigned long previousMillis = 0;

// // Buzzer control
// bool buzzerActive = false;
// unsigned long buzzerStartMillis = 0;
// const unsigned long buzzerDuration = 5000; // 5s
// void startBuzzer();
// void Warning()
// {
//   bool warning = false;

//   if (tempc <= 20)
//   {
//     Serial.println("⚠️ Nhiệt độ quá thấp");
//     warning = true;
//   }
//   if (tempc >= 38)
//   {
//     Serial.println("⚠️ Nhiệt độ quá cao");
//     warning = true;
//   }
//   if (humi <= 30)
//   {
//     Serial.println("⚠️ Độ ẩm quá thấp");
//     warning = true;
//   }
//   if (humi >= 90)
//   {
//     Serial.println("⚠️ Độ ẩm quá cao");
//     warning = true;
//   }
//   if (ppm >= 600)
//   {
//     Serial.println("⚠️ Phát hiện khí nồng độ cao!");
//     warning = true;
//   }

//   if (warning)
//     startBuzzer();
// }

// void startBuzzer()
// {
//   if (!buzzerActive)
//   {
//     buzzerActive = true;
//     buzzerStartMillis = millis();
//     tone(BUZZER_PIN, 7000); // Bắt đầu kêu
//   }
// }

// void updateBuzzer()
// {
//   if (buzzerActive && millis() - buzzerStartMillis >= buzzerDuration)
//   {
//     noTone(BUZZER_PIN); // Tắt buzzer sau 5 giây
//     buzzerActive = false;
//   }
// }

// void Wifi_Connection()
// {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(1000);
//     Serial.println("Đang kết nối WiFi...");
//   }
//   Serial.println("✅ Đã kết nối WiFi");
//   Serial.print("IP ESP32: ");
//   Serial.println(WiFi.localIP());
// }

// void setup()
// {
//   Serial.begin(9600);
//   Serial.println("Đang khởi tạo...");
//   dht.begin();
//   Wifi_Connection();
//   analogReadResolution(12);
//   pinMode(BUZZER_PIN, OUTPUT);
//   noTone(BUZZER_PIN);
// }

// void loop()
// {
//   updateBuzzer(); // kiểm tra nếu đã hết 5s thì tắt buzzer

//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= interval)
//   {
//     previousMillis = currentMillis;

//     float t = dht.readTemperature();
//     float h = dht.readHumidity();
//     int mq2_raw = analogRead(MQ2_PIN);

//     // if (isnan(t) || isnan(h))
//     // {
//     //   Serial.println("❌ Không đọc được dữ liệu từ DHT22");
//     //   return;
//     // }

//     tempc = t;
//     humi = (int)h;
//     ppm = mq2_raw;

//     String postData = "tempc=" + String(tempc) + "&humi=" + String(humi) + "&ppm=" + String(ppm);

//     HTTPClient http;
//     http.begin(URL);
//     http.addHeader("Content-Type", "application/x-www-form-urlencoded");

//     int httpCode = http.POST(postData);
//     String payload = http.getString();
//     Serial.println("URL: http://192.168.1.124/ESP32_SMART_2/index.html");
//     Serial.println("📊 Gửi dữ liệu:");
//     Serial.println("Dữ liệu: " + postData);
//     Serial.println("Mã phản hồi: " + String(httpCode));
//     Serial.println("Phản hồi: " + payload);
//     Warning();
//     Serial.println("---------------------------------");

//     http.end();
//   }
// }
// // #include "DHT.h"

// // #define DHTPIN 23     // Chân DATA nối với GPIO 23
// // #define DHTTYPE DHT22 // Loại cảm biến là DHT11

// // DHT dht(DHTPIN, DHTTYPE);

// // void setup()
// // {
// //   Serial.begin(115200);
// //   dht.begin();
// //   Serial.println("DHT11 ESP32 - Bắt đầu đọc dữ liệu...");
// // }

// // void loop()
// // {
// //   delay(2000); // Chờ 2 giây giữa các lần đọc

// //   float h = dht.readHumidity();
// //   float t = dht.readTemperature(); // Đọc nhiệt độ °C

// //   // Kiểm tra lỗi
// //   // if (isnan(h) || isnan(t))
// //   // {
// //   //   Serial.println("Lỗi khi đọc từ DHT11!");
// //   //   return;
// //   // }

// //   Serial.print("Nhiệt độ: ");
// //   Serial.print(t);
// //   Serial.print(" °C, Độ ẩm: ");
// //   Serial.print(h);
// //   Serial.println(" %");
// // }
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi Info
const char *ssid = "102 Mini";
const char *password = "khongchopass";

// Server URL
String URL = "http://172.20.10.8/ESP32_SMART_2/testdata.php";

// DHT22
#define DHTPIN 23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQ2
#define MQ2_PIN 34

// Buzzer
#define BUZZER_PIN 12

// Sensor data
float tempc = 0.0;
int humi = 0;
int ppm = 0;

// Timing
const unsigned long interval = 2000;
unsigned long previousMillis = 0;

// Buzzer control
bool buzzerActive = false;
unsigned long buzzerStartMillis = 0;
const unsigned long buzzerDuration = 5000; // 5s
void startBuzzer();
void Warning()
{
  bool warning = false;

  if (tempc <= 20)
  {
    Serial.println("⚠️ Nhiệt độ quá thấp");
    warning = true;
  }
  if (tempc >= 38)
  {
    Serial.println("⚠️ Nhiệt độ quá cao");
    warning = true;
  }
  if (humi <= 30)
  {
    Serial.println("⚠️ Độ ẩm quá thấp");
    warning = true;
  }
  if (humi >= 90)
  {
    Serial.println("⚠️ Độ ẩm quá cao");
    warning = true;
  }
  if (ppm >= 600)
  {
    Serial.println("⚠️ Phát hiện khí nồng độ cao!");
    warning = true;
  }

  if (warning)
    startBuzzer();
}

void startBuzzer()
{
  if (!buzzerActive)
  {
    buzzerActive = true;
    buzzerStartMillis = millis();
    tone(BUZZER_PIN, 7000); // Bắt đầu kêu
  }
}

void updateBuzzer()
{
  if (buzzerActive && millis() - buzzerStartMillis >= buzzerDuration)
  {
    noTone(BUZZER_PIN); // Tắt buzzer sau 5 giây
    buzzerActive = false;
  }
}

void Wifi_Connection()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
  }
  Serial.println("✅ Đã kết nối WiFi");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Đang khởi tạo...");
  dht.begin();
  Wifi_Connection();
  analogReadResolution(12);
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
}

void loop()
{
  updateBuzzer(); // kiểm tra nếu đã hết 5s thì tắt buzzer

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int mq2_raw = analogRead(MQ2_PIN);

    // if (isnan(t) || isnan(h))
    // {
    //   Serial.println("❌ Không đọc được dữ liệu từ DHT22");
    //   return;
    // }

    tempc = t;
    humi = (int)h;
    ppm = mq2_raw;

    String postData = "tempc=" + String(tempc) + "&humi=" + String(humi) + "&ppm=" + String(ppm);

    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println("URL: http://172.20.10.8/ESP32_SMART_2/index.html");
Serial.println("📊 Gửi dữ liệu:");
    Serial.println("Dữ liệu: " + postData);
    Serial.println("Mã phản hồi: " + String(httpCode));
    Serial.println("Phản hồi: " + payload);
    Warning();
    Serial.println("---------------------------------");

    http.end();
  }
}
// #include "DHT.h"

// #define DHTPIN 23     // Chân DATA nối với GPIO 23
// #define DHTTYPE DHT22 // Loại cảm biến là DHT11

// DHT dht(DHTPIN, DHTTYPE);

// void setup()
// {
//   Serial.begin(115200);
//   dht.begin();
//   Serial.println("DHT11 ESP32 - Bắt đầu đọc dữ liệu...");
// }

// void loop()
// {
//   delay(2000); // Chờ 2 giây giữa các lần đọc

//   float h = dht.readHumidity();
//   float t = dht.readTemperature(); // Đọc nhiệt độ °C

//   // Kiểm tra lỗi
//   // if (isnan(h) || isnan(t))
//   // {
//   //   Serial.println("Lỗi khi đọc từ DHT11!");
//   //   return;
//   // }

//   Serial.print("Nhiệt độ: ");
//   Serial.print(t);
//   Serial.print(" °C, Độ ẩm: ");
//   Serial.print(h);
//   Serial.println(" %");
// }