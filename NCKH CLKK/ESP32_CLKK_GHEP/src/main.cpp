#include <Arduino.h>
#include <WiFi.h>
#include "DHT.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "time.h"
#include <HTTPClient.h>
#include <WebServer.h>
#include <Preferences.h>

// ==== Real time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // múi giờ +7 (Việt Nam)
const int daylightOffset_sec = 0;

// ==== WiFi ====
const char *ssid = "Open WorkShop 2.4GHz";
const char *password = "0000*0000";

// Access Point để cấu hình lại
const char *apSSID = "ESP32_Config";
const char *apPASS = "";

WebServer server(80);
Preferences prefs;

// Giao diện web cấu hình
String htmlPage()
{
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'>"
                "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                "<title>ESP32 WiFi Config</title></head><body style='font-family:sans-serif;'>"
                "<h2>Thiết lập WiFi</h2>"
                "<form action='/save' method='post'>"
                "SSID:<br><input name='ssid' required><br><br>"
                "Mật khẩu:<br><input name='pass' type='password'><br><br>"
                "<input type='submit' value='Lưu & Kết nối'>"
                "</form></body></html>";
  return page;
}

// Xử lý lưu WiFi mới
void handleSave()
{
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();

  server.send(200, "text/html", "<h3>Đã lưu! ESP32 sẽ khởi động lại...</h3>");
  delay(2000);
  ESP.restart();
}

// Giao diện cấu hình
void startConfigPortal()
{
  Serial.println("⚙️ Bật Access Point để cấu hình WiFi...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("🔗 Kết nối vào SSID: ");
  Serial.println(apSSID);
  Serial.print("🌐 Truy cập: http://");
  Serial.println(IP);

  server.on("/", []()
            { server.send(200, "text/html", htmlPage()); });
  server.on("/save", HTTP_POST, handleSave);
  server.begin();

  while (WiFi.status() != WL_CONNECTED)
  {
    server.handleClient();
    delay(10);
  }
}
// ==== HiveMQ Cloud ====
const char *firebase_host = "https://nckh-clkk-default-rtdb.asia-southeast1.firebasedatabase.app/";
String firebase_path = "sensor_data.json";

// ==== TFT ====
TFT_eSPI tft = TFT_eSPI();

// ==== DHT22 ====
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== MQ135 ====  đo NH3 phải nhỏ hơn 0.1
const int MQ135_PIN = 34;  // Chân ADC đọc MQ-135
const float RL = 10000.0;  // Điện trở tải (10kΩ)
const float VCC = 5.0;     // Điện áp nuôi cảm biến
const float ADC_REF = 3.3; // Điện áp tham chiếu ADC ESP32
const int ADC_MAX = 4095;  // 12-bit ADC
float R0 = 10000.0;        // Giá trị R0 (hiệu chuẩn trong không khí sạch)

// ==== Đường cong theo datasheet (log-log) ====
// log10(ppm) = (log10(Rs/R0) - b) / m
// hoặc ppm = 10^((log10(Rs/R0) - b) / m)
const float M_NH3 = -0.48; // Hệ số slope NH3 (xấp xỉ)
const float B_NH3 = 0.35;  // intercept NH3

int samples = 10; // lọc trung bình

float readVoltage()
{
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++)
  {
    sum += analogRead(MQ135_PIN);
    delay(10);
  }
  float adc = (float)sum / samples;
  return (adc / ADC_MAX) * ADC_REF;
}

float calcRs(float vout)
{
  if (vout <= 0.0001)
    return 1e6;
  return RL * (VCC - vout) / vout;
}

float RsRo_to_ppm(float rs_ro, float m, float b)
{
  if (rs_ro <= 0)
    return -1;
  float log_ppm = (log10(rs_ro) - b) / m;
  return pow(10, log_ppm);
}

// ==== MQ7 ====  ok là 0-9 CO đang đạt
#define MQ7_PIN 32
#define RL_7 10000
#define R0_7 10000

float getMQ7Resistance(int adcValue)
{
  float sensorVoltage = ((float)adcValue / 4095.0) * 3.3;
  float rs = (3.3 - sensorVoltage) * RL_7 / sensorVoltage;
  return rs;
}

float getCOppm(float rs)
{
  float ratio = rs / R0_7;
  float m = -0.77;
  float b = 1.699;
  float log_ppm = (log10(ratio) - b) / m;
  return pow(10, log_ppm);
}

//-- Define Bụi mịn GP2Y --  bụi 1-10 là đẹp
#define GP2Y_ADC 33
#define GP2Y_LED 13

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

//-- Hết define Bụi mịn GP2Y --
void GP2Y_setup()
{
  pinMode(GP2Y_LED, OUTPUT);
}
float Run_GP2Y()
{
  float sumDust = 0;

  for (int i = 0; i < 100; i++)
  {
    digitalWrite(GP2Y_LED, LOW);
    delayMicroseconds(samplingTime);
    voMeasured = analogRead(GP2Y_ADC);
    delayMicroseconds(deltaTime);
    digitalWrite(GP2Y_LED, HIGH);
    delayMicroseconds(sleepTime);

    calcVoltage = voMeasured * (5.0 / 1024.0);
    dustDensity = 0.17 * calcVoltage - 0.1; // mg/m3

    sumDust += dustDensity;
    delay(100);
  }
  sumDust /= 10.0;
  float avgDust_mg = sumDust / 100.0;     // mg/m3
  float avgDust_ug = avgDust_mg * 1000.0; // µg/m3
  Serial.print("Bui trung binh: ");
  Serial.print(avgDust_mg, 3);
  Serial.print(" mg/m3 (");
  Serial.print(avgDust_ug, 1);
  Serial.println(" µg/m3)");
  return avgDust_ug;
}

// ==== Biến dữ liệu ====
float temp, humi, co_ppm, nh3_ppm, dust;

// ==== Firebase ====

String getTimestamp()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
    return "Unknown";

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}
void sendToFirebase()
{
  String timestamp = getTimestamp(); // "2025-11-06 15:30:12"

  // Tạo chuỗi ID gọn gàng hơn (Firebase không cho dấu cách, dấu hai chấm)
  String recordID = timestamp;
  recordID.replace(" ", "_");
  recordID.replace(":", "-");

  String jsonData = "{";
  jsonData += "\"time\":\"" + timestamp + "\",";
  jsonData += "\"temp\":" + String(temp, 2) + ",";
  jsonData += "\"humi\":" + String(humi, 2) + ",";
  jsonData += "\"co\":" + String(co_ppm, 2) + ",";
  jsonData += "\"nh3\":" + String(nh3_ppm, 2) + ",";
  jsonData += "\"dust\":" + String(dust, 2);
  jsonData += "}";

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // Ghi dữ liệu vào nhánh sensor_data/<timestamp>.json
    String url = String(firebase_host) + "sensor_data/" + recordID + ".json";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.PUT(jsonData); // PUT để ghi đúng ID

    if (httpResponseCode > 0)
      Serial.println("📤 Firebase OK: " + String(httpResponseCode) + " (" + recordID + ")");
    else
      Serial.println("❌ Firebase lỗi: " + http.errorToString(httpResponseCode));

    http.end();
  }
}

// ==== Ngưỡng cảnh báo ====
#define TEMP_HIGH 35.0
#define HUMI_LOW 30.0
#define HUMI_HIGH 90.0
#define CO_HIGH 50.0
#define NH3_HIGH 25.0

int collectWarnings(String warnList[], int maxItems)
{
  int count = 0;
  if (temp > TEMP_HIGH && count < maxItems)
    warnList[count++] = "HIGH TEMP!";
  if ((humi < HUMI_LOW || humi > HUMI_HIGH) && count < maxItems)
    warnList[count++] = "ABN HUMI!";
  if (co_ppm > CO_HIGH && count < maxItems)
    warnList[count++] = "HIGH CO!";
  if (nh3_ppm > NH3_HIGH && count < maxItems)
    warnList[count++] = "HIGH NH3!";
  return count;
}

// ==== TFT hiển thị ====
void drawScreen()
{
  int w = tft.width();
  int h = tft.height();
  int boxW = w / 2;
  int boxH = h / 2;

  String warns[4];
  int warnCount = collectWarnings(warns, 4);
  bool warning = (warnCount > 0);

  uint16_t bg = warning ? TFT_RED : TFT_BLUE;
  tft.fillScreen(bg);
  tft.drawLine(boxW, 0, boxW, h, TFT_WHITE);
  tft.drawLine(0, boxH, w, boxH, TFT_WHITE);
  tft.drawRect(0, 0, w, h, TFT_WHITE);

  tft.setTextColor(TFT_WHITE, bg);
  tft.setTextSize(2);
  char buf[32];

  sprintf(buf, "%.1f C", temp);
  tft.setCursor(20, 30);
  tft.print("TEMP ");
  tft.print(buf);

  sprintf(buf, "%.1f %%", humi);
  tft.setCursor(20, 80);
  tft.print("HUMI ");
  tft.print(buf);

  if (warning)
  {
    tft.setTextColor(TFT_YELLOW, bg);
    int x = boxW + 10;
    int y = 20;
    for (int i = 0; i < warnCount; i++)
    {
      tft.setCursor(x, y + i * 25);
      tft.print(warns[i]);
    }
  }
  else
  {
    tft.setTextColor(TFT_WHITE, bg);
    tft.setCursor(boxW + 30, boxH / 2 - 10);
    tft.print("OK");
  }

  sprintf(buf, "%.1f ppm", co_ppm);
  tft.setTextColor(TFT_WHITE, bg);
  tft.setCursor(20, boxH + 20);
  tft.print("CO ");
  tft.print(buf);

  sprintf(buf, "%.1f ppm", nh3_ppm);
  tft.setCursor(boxW + 20, boxH + 40);
  tft.print("NH3 ");
  tft.print(buf);

  sprintf(buf, "%.1f ug/m3", dust);
  tft.setCursor(20, boxH + 60);
  tft.print("Dust ");
  tft.setCursor(20, boxH + 90);
  tft.print(buf);
}
void TFT_init()
{
  tft.init();
  tft.setRotation(1);
}
// Hàm khởi động WiFi thông minh
void Wifi_init()
{
  Serial.println("🚀 Khởi động WiFi...");

  // 1️⃣ Thử dùng Wi-Fi mặc định
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("📶 Đang thử kết nối WiFi mặc định: %s\n", ssid);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 15)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }

  // 2️⃣ Nếu kết nối thành công → xong
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\n✅ Kết nối WiFi mặc định thành công!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("🕒 Đang đồng bộ thời gian NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    return;
  }

  // 3️⃣ Nếu thất bại → thử Wi-Fi đã lưu trong bộ nhớ
  Serial.println("\n⚠️ Không kết nối được WiFi mặc định, thử Wi-Fi đã lưu...");
  prefs.begin("wifi", true);
  String saved_ssid = prefs.getString("ssid", "");
  String saved_pass = prefs.getString("pass", "");

  prefs.end();

  if (saved_ssid != "")
  {
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());
    Serial.printf("📡 Đang thử kết nối WiFi đã lưu: %s\n", saved_ssid.c_str());
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 15)
    {
      delay(500);
      Serial.print(".");
      retry++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\n✅ Kết nối WiFi đã lưu thành công!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      return;
    }
  }

  // 4️⃣ Nếu vẫn thất bại → mở Access Point cấu hình
  Serial.println("\n❌ Không kết nối được, chuyển sang chế độ cấu hình.");
  startConfigPortal();
}

void MQ135_Init()
{
  analogSetPinAttenuation(MQ135_PIN, ADC_11db);
  pinMode(MQ135_PIN, INPUT);
}
void MQ7_Init()
{
  analogSetPinAttenuation(MQ7_PIN, ADC_11db);
  pinMode(MQ7_PIN, INPUT);
}

void DHT22_run()
{
  humi = dht.readHumidity();
  temp = dht.readTemperature();
}
void MQ135_run()
{
  float vout = readVoltage();
  float Rs = calcRs(vout);
  float ratio = Rs / R0;

  nh3_ppm = RsRo_to_ppm(ratio, M_NH3, B_NH3);
}
void MQ7_run()
{
  int adc7 = analogRead(MQ7_PIN);
  float Rs7 = getMQ7Resistance(adc7);
  co_ppm = getCOppm(Rs7);
}
// ==== Setup ====
void setup()
{
  Serial.begin(9600);
  Wifi_init();
  MQ135_Init();
  MQ7_Init();
  GP2Y_setup();
  dht.begin();
  TFT_init();
}

// ==== Loop ====
void loop()
{
  DHT22_run();
  MQ135_run();
  MQ7_run();
  dust = Run_GP2Y();
  drawScreen();
  sendToFirebase();
  delay(586000);
}
