#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ==== Cấu hình cảm biến ====
#define DHTPIN 4 // GPIO4
#define DHTTYPE DHT22
#define MQ2_PIN 34 // GPIO34 - chân ADC cho MQ2

// ==== Thông tin WiFi ====
const char *ssid = "102 Mini";
const char *password = "khongchopass";

// ==== URL PHP để ghi dữ liệu ====
const char *postServer = "http://192.168.1.124/testdata.php"; // 👈 Đúng tên file POST

// ==== Khởi tạo DHT ====
DHT dht(DHTPIN, DHTTYPE);

// Thời gian gửi dữ liệu
unsigned long lastSend = 0;
const unsigned long interval = 5000; // gửi mỗi 5 giây

void setup()
{
  Serial.begin(115200);
  dht.begin();

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Đã kết nối WiFi!");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println("http://192.168.1.124/ESP32_SmartHome_Web/index.html");
}

void loop()
{
  unsigned long now = millis();
  if (now - lastSend >= interval)
  {
    lastSend = now;

    float temp = dht.readTemperature();
    float humi = dht.readHumidity();
    int mq2 = analogRead(MQ2_PIN);

    // Kiểm tra lỗi cảm biến
    if (isnan(temp) || isnan(humi))
    {
      Serial.println("⚠️ Không đọc được từ cảm biến DHT22!");
      return;
    }

    Serial.printf("Đọc được: Temp=%.1f°C, Humi=%.1f%%, MQ2=%d\n", temp, humi, mq2);

    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;
      http.begin(postServer);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Tạo dữ liệu POST
      String postData = "Temp=" + String(temp, 1) + "&Humi=" + String(humi, 1) + "&Mq2=" + String(mq2);

      int httpCode = http.POST(postData);
      String payload = http.getString();

      Serial.printf("📡 Gửi POST: %s\n", postData.c_str());
      Serial.printf("📥 Phản hồi (%d): %s\n", httpCode, payload.c_str());

      http.end();
    }
    else
    {
      Serial.println("❌ Mất WiFi!");
    }
  }

  delay(100); // Cho CPU nghỉ nhẹ
}
