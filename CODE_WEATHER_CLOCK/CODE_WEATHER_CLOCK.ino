#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include <time.h>
#include <animation.h>
#include <animation1.h>
#include <animation2.h>
#include <animation3.h>
#include <animation4.h>
#include <animation5.h>
#include <TFT_eSPI.h>  // chỉ dùng TFT_eSPI


// ===== Wi-Fi =====
const char* WIFI_SSID = "Xuong";
const char* WIFI_PASS = "68686868";

// ===== Open-Meteo (Hà Nội) =====
const char* OM_URL_HTTP_NEW =
  "http://api.open-meteo.com/v1/forecast"
  "?latitude=21.0285&longitude=105.8542"
  "&current=temperature_2m,relative_humidity_2m,weather_code"
  "&timezone=Asia%2FBangkok";

const char* OM_URL_HTTP_OLD =
  "http://api.open-meteo.com/v1/forecast"
  "?latitude=21.0285&longitude=105.8542"
  "&current_weather=true"
  "&timezone=Asia%2FBangkok";

// ===== TFT pins =====
#define TFT_CS  5
#define TFT_DC  21
#define TFT_RST 22
#define TFT_BL  25
#define TFT_BACKLIGHT_ON HIGH
TFT_eSPI tft = TFT_eSPI();


// ===== Bluetooth SPP =====
BluetoothSerial BT;

// ===== UI helpers =====
void clearScreen() { 
  tft.fillScreen(TFT_BLACK); 
}

void showText(const String& title, const String& body) {
  clearScreen();
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  // Tiêu đề
  tft.drawString(title, 10, 20, 2);  // font 4 to, rõ

  // Nội dung
  tft.setTextSize(1);
  tft.drawString(body, 10, 70, 1);
}

// void showText(const char *msg, int x, int y, int font = 2, uint16_t color = TFT_WHITE, uint16_t bgcolor = TFT_BLACK) {
//   tft.setTextDatum(TL_DATUM);   // Top Left
//   tft.setTextFont(font);
//   tft.setTextColor(color, bgcolor);
//   tft.drawString(msg, x, y);
// }

void showStatusWifi()    { Serial.println("[UI] Setup: Ket noi WiFi..."); showText("Setup", "Ket noi WiFi..."); }
void showStatusBTWait()  { Serial.println("[UI] Setup: Cho ket noi Bluetooth..."); showText("Setup", "Cho ket noi Bluetooth..."); }
void showErr(const String& m) { Serial.printf("[UI] Error: %s\n", m.c_str()); showText("Error", m); }
void showUnknown(const String& raw) { Serial.printf("[UI] Unknown cmd: %s\n", raw.c_str()); showText("Khong biet", "Lenh: " + raw); }

// void showCenteredECBOT() {
//   clearScreen();
//   const char* s = "EC BOT";
//   int sz = 4;
//   int w = 320, h = 240;
//   int charW = 6 * sz, charH = 8 * sz;
//   int textW = strlen(s) * charW;
//   int x = (w - textW) / 2;
//   int y = (h - charH) / 2;
//   tftt.setTextColor(ILI9341_WHITE);
//   tftt.setTextSize(sz);
//   tftt.setCursor(x, y);
//   tftt.println(s);
//   Serial.println("[UI] Show: EC BOT (idle)");
// }

// Xin chao vietnam
void showAnimation() {
  for (int i = 0; i < frames; i++) {
    delay(250);
    tft.pushImage(0, 0, animation_width, animation_height, animation[i]);
  }
}

// Emotion: happy
void showAnimation1() {
  for (int i = 0; i < frames1; i++) {
    delay(250);
    tft.pushImage(0, 0, animation1_width, animation1_height, animation1[i]);
  }
}

// Emotion: sad
void showAnimation2() {
  for (int i = 0; i < frames2; i++) {
    delay(250);
    tft.pushImage(0, 0, animation2_width, animation2_height, animation2[i]);
  }
}

// Emotion: dac biet
void showAnimation3() {
  for (int i = 0; i < frames3; i++) {
    delay(250);
    tft.pushImage(0, 0, animation3_width, animation3_height, animation3[i]);
  }
}

// logo EC
void showAnimation4() {
  for (int i = 0; i < frames4; i++) {
    delay(250);
    tft.pushImage(0, 0, animation4_width, animation4_height, animation4[i]);
  }
}

// ma qr
void showAnimation5() {
  for (int i = 0; i < frames5; i++) {
    delay(250);
    tft.pushImage(0, 0, animation5_width, animation5_height, animation5[i]);
  }
}


// void showWeatherUI(float tC, int hum, const String& desc) {/
//   String body = "Temp: " + String(tC, 1) + "*C\n";
//   if (hum >= 0) body += "Hum : " + String(hum) + "%\n";
//   body += "Desc: " + desc;
//   showText("Hanoi Weather", body);
//   Serial.printf("[UI] Weather -> Temp=%.1fC Hum=%d Desc=%s\n", tC, hum, desc.c_str());
// }

void showWeatherUI(float tC, int hum, const String& desc) {
  clearScreen();
  tft.fillScreen(TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setTextFont(2);
  tft.setTextDatum(TL_DATUM);
  tft.drawCentreString("Hanoi Weather", 160, 20, 4);
  // tft.drawString("Hanoi Weather", 10, 20, 4);

  int y = 70;
  tft.drawString("Temp: " + String(tC, 1) + "*C", 10, y, 4); y += 30;
  if (hum >= 0) {
    tft.drawString("Hum : " + String(hum) + "%", 10, y, 4); y += 30;
  }
  tft.drawString("Desc: " + desc, 10, y, 4);
}


// ===== Clock (GMT+7) =====
const long GMT7_OFFSET = 7L * 3600L;
bool timeConfigured = false;

void ensureTime() {
  if (timeConfigured) return;
  Serial.println("[Time] Config NTP (GMT+7)...");
  configTime(GMT7_OFFSET, 0, "pool.ntp.org", "time.nist.gov");
  struct tm tmNow;
  for (int i = 0; i < 25; ++i) {
    if (getLocalTime(&tmNow, 200)) { timeConfigured = true; break; }
  }
  Serial.printf("[Time] Sync %s\n", timeConfigured ? "OK" : "FAILED");
}

void showClockNow() {
  ensureTime();
  struct tm tmNow;
  if (!getLocalTime(&tmNow, 200)) { showErr("Time sync failed"); return; }

  char lineTime[16], lineDate[32], lineWeekday[16];
  snprintf(lineTime, sizeof(lineTime), "%02d:%02d:%02d", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
  snprintf(lineDate, sizeof(lineDate), "%02d/%02d/%04d", tmNow.tm_mday, tmNow.tm_mon + 1, tmNow.tm_year + 1900);

  // Tạo chuỗi thứ
  const char* weekdays[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  snprintf(lineWeekday, sizeof(lineWeekday), "%s", weekdays[tmNow.tm_wday]);

  // clearScreen();
  tft.fillScreen(TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);

  // Giờ to – nằm giữa màn hình
  tft.drawCentreString(lineTime, 160, 70, 7);  // 320/2=160 (ngang), 70px từ trên

  // Thứ ở ngay dưới giờ
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.drawCentreString(lineWeekday, 160, 150, 4);

  // Ngày tháng năm – nhỏ hơn, dưới cùng
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.drawCentreString(lineDate, 160, 190, 4);

  Serial.printf("[UI] Clock -> %s %s %s\n", lineTime, lineWeekday, lineDate);
}


// void showClockNow() {
//   ensureTime();
//   struct tm tmNow;
//   if (!getLocalTime(&tmNow, 200)) { showErr("Time sync failed"); return; }
//   char line1[16], line2[32];
//   snprintf(line1, sizeof(line1), "%02d:%02d:%02d", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
//   snprintf(line2, sizeof(line2), "%02d/%02d/%04d GMT+7", tmNow.tm_mday, tmNow.tm_mon + 1, tmNow.tm_year + 1900);

//   clearScreen();

//   // Dòng giờ to, căn giữa
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.drawCentreString(line1, 160, 90, 7); // font số to

//   // Dòng ngày nhỏ hơn
//   tft.drawCentreString(line2, 160, 160, 4);

//   Serial.printf("[UI] Clock -> %s %s\n", line1, line2);
// }


// ===== Wi-Fi =====
bool ensureWiFi(uint32_t timeoutMs = 8000) {
  if (WiFi.status() == WL_CONNECTED) return true;
  showStatusWifi();
  Serial.printf("[WiFi] Connecting to SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < timeoutMs) {
    delay(200);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] Connected. IP=%s RSSI=%d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
    return true;
  }
  Serial.println("[WiFi] Connect failed");
  return false;
}

bool internetOpen() {
  HTTPClient http;
  http.begin("http://clients3.google.com/generate_204");
  int code = http.GET();
  http.end();
  Serial.printf("[PortalCheck] %d (204=OK)\n", code);
  return code == 204;
}

// ===== WMO → mô tả ngắn =====
String wmoDesc(int code) {
  if (code == 0) return "Clear sky";
  if (code >= 1 && code <= 3) return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if (code == 51 || code == 53 || code == 55) return "Drizzle";
  if (code == 56 || code == 57) return "Freezing drizzle";
  if (code == 61 || code == 63 || code == 65) return "Rain";
  if (code == 66 || code == 67) return "Freezing rain";
  if (code == 71 || code == 73 || code == 75) return "Snow";
  if (code == 77) return "Snow grains";
  if (code == 80 || code == 81 || code == 82) return "Rain showers";
  if (code == 85 || code == 86) return "Snow showers";
  if (code == 95) return "Thunderstorm";
  if (code == 96 || code == 99) return "Thunderstorm w/ hail";
  return "WMO " + String(code);
}

// ===== HTTP + ArduinoJson (lọc trường) =====
bool httpGetAndParseCurrent(const char* url, float& tC, int& hum, int& wmo) {
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setTimeout(8000);
  http.useHTTP10(true);
  http.addHeader("Accept-Encoding", "identity");

  if (!http.begin(url)) return false;
  int code = http.GET();
  Serial.printf("[HTTP GET] %s -> %d\n", url, code);
  if (code != 200) { http.end(); return false; }

  StaticJsonDocument<256> filterNew;
  filterNew["current"]["temperature_2m"] = true;
  filterNew["current"]["relative_humidity_2m"] = true;
  filterNew["current"]["weather_code"] = true;

  DynamicJsonDocument doc(512);
  DeserializationError err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filterNew));
  http.end();

  if (err) {
    Serial.printf("[JSON] Parse error: %s\n", err.c_str());
    return false;
  }
  if (doc.containsKey("current")) {
    JsonObject cur = doc["current"];
    tC  = cur["temperature_2m"]       | NAN;
    hum = cur["relative_humidity_2m"] | -1;
    wmo = cur["weather_code"]         | -1;
    Serial.printf("[JSON] current: temp=%.1fC hum=%d wmo=%d\n", tC, hum, wmo);
    return !isnan(tC);
  }
  Serial.println("[JSON] 'current' not found");
  return false;
}

bool httpGetAndParseCurrentWeather(const char* url, float& tC, int& hum, int& wmo) {
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setTimeout(8000);
  http.useHTTP10(true);
  http.addHeader("Accept-Encoding", "identity");

  if (!http.begin(url)) return false;
  int code = http.GET();
  Serial.printf("[HTTP GET] %s -> %d\n", url, code);
  if (code != 200) { http.end(); return false; }

  StaticJsonDocument<192> filterOld;
  filterOld["current_weather"]["temperature"] = true;
  filterOld["current_weather"]["weathercode"] = true;

  DynamicJsonDocument doc(384);
  DeserializationError err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filterOld));
  http.end();

  if (err) {
    Serial.printf("[JSON] Parse error: %s\n", err.c_str());
    return false;
  }
  if (doc.containsKey("current_weather")) {
    JsonObject cw = doc["current_weather"];
    tC  = cw["temperature"] | NAN;
    wmo = cw["weathercode"] | -1;
    hum = -1;
    Serial.printf("[JSON] current_weather: temp=%.1fC wmo=%d\n", tC, wmo);
    return !isnan(tC);
  }
  Serial.println("[JSON] 'current_weather' not found");
  return false;
}

bool isAnimationCmd(String s) {
  s.trim(); s.toLowerCase();
  return (s == "việt nam") || (s == "tôi yêu việt nam") || (s == "quốc kỳ")|| (s == "quốc kì");
}

bool isAnimationCmd1(String s) {
  s.trim(); s.toLowerCase();
  return (s == "vui") || (s == "cười");
}

bool isAnimationCmd2(String s) {
  s.trim(); s.toLowerCase();
  return (s == "khóc") || (s == "buồn");
}

bool isAnimationCmd3(String s) {
  s.trim(); s.toLowerCase();
  return (s == "háo hức") || (s == "xin chào")|| (s == "đặc biệt");
}

bool isAnimationCmd5(String s) {
  s.trim(); s.toLowerCase();
  return (s == "mã")||(s == "đơn đăng ký");
}

bool fetchWeatherHanoi(float& outTempC, int& outHumidity, String& outDesc) {
  if (!ensureWiFi()) return false;
  if (!internetOpen()) return false;

  float tC; int hum; int wmo;

  if (httpGetAndParseCurrent(OM_URL_HTTP_NEW, tC, hum, wmo)) {
    outTempC = tC; outHumidity = hum; outDesc = wmoDesc(wmo);
    Serial.printf("[Weather] OK (new) -> %.1fC, hum=%d, desc=%s\n",
                  outTempC, outHumidity, outDesc.c_str());
    return true;
  }
  if (httpGetAndParseCurrentWeather(OM_URL_HTTP_OLD, tC, hum, wmo)) {
    outTempC = tC; outHumidity = hum; outDesc = wmoDesc(wmo);
    Serial.printf("[Weather] OK (old) -> %.1fC, hum=%d, desc=%s\n",
                  outTempC, outHumidity, outDesc.c_str());
    return true;
  }
  Serial.println("[Weather] Fetch failed (both variants)");
  return false;
}

// ===== Command logic =====
bool isWeatherCmd(String s) {
  s.trim(); s.toLowerCase();
  return (s == "thời tiết") || (s == "dự báo") || (s == "dự báo thời tiết") || (s == "thời tiết hôm nay");
}
bool isClockCmd(String s) {
  s.trim(); s.toLowerCase();
  return (s == "đồng hồ") || (s == "xem giờ") ||
         (s == "bây giờ là mấy giờ") || (s == "mấy giờ");
}

// enum DisplayMode { MODE_IDLE, MODE_CLOCK, MODE_WEATHER, MODE_UNKNOWN };
enum DisplayMode { MODE_IDLE, MODE_CLOCK, MODE_WEATHER, MODE_ANIMATION, MODE_ANIMATION1, MODE_ANIMATION2, MODE_ANIMATION3, MODE_ANIMATION5, MODE_UNKNOWN };

DisplayMode mode = MODE_IDLE;
unsigned long lastClockUpdateMs = 0;
String rxBuf; 
unsigned long lastByteMs = 0;
const unsigned long LINE_TIMEOUT_MS = 400;

void handleText(const String& sRaw) {
  Serial.printf("[BT] RX: %s\n", sRaw.c_str());
  if (isClockCmd(sRaw)) {
    mode = MODE_CLOCK;
    Serial.println("[CMD] CLOCK");
    showClockNow();
    BT.println("OK");
  } else if (isWeatherCmd(sRaw)) {
    Serial.println("[CMD] WEATHER");
    float t; int h; String desc;
    if (fetchWeatherHanoi(t, h, desc)) { showWeatherUI(t, h, desc); mode = MODE_WEATHER; BT.println("OK"); }
    else                                { showErr("Fetch failed");  mode = MODE_UNKNOWN; BT.println("ERR");}
  } else if (isAnimationCmd(sRaw)) {
    Serial.println("[CMD] ANIMATION");
    mode = MODE_ANIMATION;
    showAnimation();
    BT.println("OK");
  } else if (isAnimationCmd1(sRaw)) {
    Serial.println("[CMD] ANIMATION1");
    mode = MODE_ANIMATION1;
    showAnimation1();
    BT.println("OK");
  } else if (isAnimationCmd2(sRaw)) {
    Serial.println("[CMD] ANIMATION2");
    mode = MODE_ANIMATION2;
    showAnimation2();
    BT.println("OK");
  } else if (isAnimationCmd3(sRaw)) {
    Serial.println("[CMD] ANIMATION3");
    mode = MODE_ANIMATION3;
    showAnimation3();
    BT.println("OK");
  } else if (isAnimationCmd5(sRaw)) {
    Serial.println("[CMD] ANIMATION5");
    mode = MODE_ANIMATION5;
    showAnimation5();
    BT.println("OK");
  }
  else {
    Serial.println("[CMD] UNKNOWN");
    showUnknown(sRaw);
    mode = MODE_UNKNOWN;
    BT.println("UNKNOWN");
  }
}

// ===== BT callbacks =====
void btEvent(esp_spp_cb_event_t ev, esp_spp_cb_param_t*) {
  if (ev == ESP_SPP_SRV_OPEN_EVT)  { Serial.println("[BT] Client connected");  showAnimation4(); mode = MODE_IDLE; }
  if (ev == ESP_SPP_CLOSE_EVT)     { Serial.println("[BT] Client disconnected"); showStatusBTWait(); mode = MODE_IDLE; }
}

// ===== Arduino entry =====
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] Starting...");

  // TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  // tftt.begin();
  // tftt.setRotation(1);

  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_WHITE);

  Serial.println("[BOOT] TFT ready");

  // Wi-Fi
  showStatusWifi();
  if (ensureWiFi()) Serial.println("[BOOT] WiFi ready");
  else              Serial.println("[BOOT] WiFi not connected (will retry on demand)");

  // Bluetooth
  showStatusBTWait();
  if (!BT.begin("ESP32_SPP")) {
    Serial.println("[BOOT] BT start failed!");
    while (true) {}
  }
  BT.register_callback(btEvent);
  Serial.println("[BOOT] ESP32 SPP ready");

  // Idle screen
  showAnimation4();
  mode = MODE_IDLE;
}

void loop() {
  // Nhận lệnh theo dòng
  while (BT.available()) {
    char c = (char)BT.read();
    if (c == '\r') continue;
    if (c == '\n') { if (rxBuf.length()) handleText(rxBuf); rxBuf = ""; }
    else { rxBuf += c; lastByteMs = millis(); }
  }
  if (rxBuf.length() && (millis() - lastByteMs > LINE_TIMEOUT_MS)) { handleText(rxBuf); rxBuf = ""; }

  // Cập nhật đồng hồ mỗi 1 giây khi ở MODE_CLOCK
  if (mode == MODE_CLOCK && millis() - lastClockUpdateMs > 1000) {
    showClockNow();
    lastClockUpdateMs = millis();
  }
  // Khi ở chế độ animation thì lặp liên tục
  if (mode == MODE_ANIMATION) {
    showAnimation();
  }
  if (mode == MODE_ANIMATION1) {
    showAnimation1();
  }
  if (mode == MODE_ANIMATION2) {
    showAnimation2();
  }
  if (mode == MODE_ANIMATION3) {
    showAnimation3();
  }
  if (mode == MODE_ANIMATION5) {
    showAnimation5();
  }
}
