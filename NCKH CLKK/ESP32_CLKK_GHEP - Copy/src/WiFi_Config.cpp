#include "Wifi_Config.h"

// ==== Real time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// ==== WiFi mặc định ====
const char *ssid = "Open WorkShop 2.4GHz";
const char *password = "0000*0000";

// Access Point
const char *apSSID = "ESP32_Config";
const char *apPASS = "";

// Global server + prefs
WebServer server(80);
Preferences prefs;

// ==== AI Alert Server ====
String receivedAlerts[10]; // Lưu tối đa 10 cảnh báo
int receivedAlertCount = 0;

// ==== HTML cấu hình WiFi ====
String htmlPage()
{
  String page =
      "<!DOCTYPE html><html><head><meta charset='utf-8'>"
      "<meta name='viewport' content='width=device-width,initial-scale=1'>"
      "<title>ESP32 WiFi Config</title></head><body>"
      "<h2>Thiết lập WiFi</h2>"
      "<form action='/save' method='post'>"
      "SSID:<br><input name='ssid' required><br><br>"
      "Mật khẩu:<br><input name='pass' type='password'><br><br>"
      "<input type='submit' value='Lưu & Kết nối'>"
      "</form></body></html>";
  return page;
}

// ==== Lưu WiFi mới ====
void handleSave()
{
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();

  server.send(200, "text/html", "<h3>Đã lưu! ESP32 đang reboot...</h3>");
  delay(1500);
  ESP.restart();
}

// ==== AP cấu hình ====
void startConfigPortal()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPASS);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
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

// ==== WiFi Init ====
void Wifi_init()
{
  Serial.println("🚀 Khởi động WiFi...");

  // 1. Thử WiFi mặc định
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int retry = 0;

  while (WiFi.status() != WL_CONNECTED && retry < 15)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected default WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("🕒 Đang đồng bộ thời gian NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    return;
  }

  // 2. Thử WiFi đã lưu
  prefs.begin("wifi", true);
  String saved_ssid = prefs.getString("ssid", "");
  String saved_pass = prefs.getString("pass", "");
  prefs.end();

  if (saved_ssid != "")
  {
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());
    retry = 0;

    while (WiFi.status() != WL_CONNECTED && retry < 15)
    {
      delay(500);
      Serial.print(".");
      retry++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nConnected saved WiFi!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      Serial.println("🕒 Đang đồng bộ thời gian NTP...");
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      return;
    }
  }

  // 3. Thất bại → vào AP cấu hình
  Serial.println("\nKhông kết nối được → mở AP cấu hình");
  startConfigPortal();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void AI_Start()
{
  server.on("/alert", HTTP_POST, []()
            {
    if (!server.hasArg("plain"))
    {
      server.send(400, "text/plain", "Body missing");
      return;
    }

    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, server.arg("plain")))
    {
      server.send(400, "text/plain", "JSON Error");
      return;
    }

    JsonArray anomalies = doc["anomalies"];

    for (JsonVariant v : anomalies)
    {
      String alert = v.as<const char *>();
      Serial.println("⚠️ Alert: " + alert);

      if (receivedAlertCount < 10)
        receivedAlerts[receivedAlertCount++] = alert;
      else
      {
        for (int i = 1; i < 10; i++)
          receivedAlerts[i - 1] = receivedAlerts[i];
        receivedAlerts[9] = alert;
      }
    }

    server.send(200, "text/plain", "OK"); });
}
