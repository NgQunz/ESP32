#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>

// ---------- WiFi ----------
const char *ssid = "Open WorkShop 2.4GHz";
const char *password = "0000*0000";

// ---------- LCD I2C ----------
#define I2C_SDA 21
#define I2C_SCL 22
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ---------- LED ----------
#define LED_OK_GR 2
#define LED_NOTOK_RED 15

// ---------- Buzzer ----------
#define BUZZER 19

// ---------- DHT11 ----------
#define DHT_DATA 4
#define DHTTYPE DHT11
DHT dht(DHT_DATA, DHTTYPE);

// ---------- MQ9 ----------
#define MQ9_A0 34
#define MQ9_D0 32

// ---------- Ngưỡng ----------
#define TEMP_THRESHOLD 50.0
#define MQ9_THRESHOLD 400.0

// Web server
WebServer server(80);

// Biến lưu trạng thái sensor
float t = 0, h = 0, mq9_ppm = 0;
bool danger = false;

// Hàm đọc cảm biến
void readSensors()
{
  h = dht.readHumidity();
  t = dht.readTemperature();

  // Đọc MQ9 (trung bình 10 lần)
  long sum = 0;
  for (int i = 0; i < 10; i++)
  {
    sum += analogRead(MQ9_A0);
    delay(5);
  }
  mq9_ppm = (sum / 10.0) / 4095.0 * 1000.0;

  // Xác định nguy hiểm
  danger = (t >= TEMP_THRESHOLD || mq9_ppm >= MQ9_THRESHOLD);
}

// Cập nhật LCD & LED
void updateLCD()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(h, 1);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("G:");
  lcd.print(mq9_ppm, 0);
  lcd.print("ppm ");

  if (danger)
  {
    lcd.print("ALM");
    digitalWrite(LED_OK_GR, LOW);
    digitalWrite(LED_NOTOK_RED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }
  else
  {
    lcd.print("SAFE");
    digitalWrite(LED_OK_GR, HIGH);
    digitalWrite(LED_NOTOK_RED, LOW);
    digitalWrite(BUZZER, LOW);
  }
}

//////////////////////////////////////////////////////////
// Web routes
void handleRoot()
{
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <title>ESP32 Fire Monitor</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
      body { font-family: Arial, sans-serif; text-align: center; background: #f9f9f9; }
      h2 { color: #333; }
      #alert { font-size: 20px; font-weight: bold; margin: 10px; padding: 10px; border-radius: 8px; display: inline-block; }
      #alert.safe { background: #c8f7c5; color: #2d862d; }
      #alert.danger { background: #f7c5c5; color: #a80000; }
      canvas { max-width: 600px; margin: 20px auto; background: #fff; border-radius: 10px; padding: 10px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }
    </style>
  </head>
  <body>
    <h2>🔥 Fire Monitoring Dashboard</h2>
    <p><b>Temp:</b> <span id="temp">--</span> °C | 
       <b>Humidity:</b> <span id="humi">--</span> % | 
       <b>Gas:</b> <span id="gas">--</span> ppm</p>

    <div id="alert" class="safe">✅ System Safe</div>

    <canvas id="chart" width="600" height="300"></canvas>

    <script>
      const ctx = document.getElementById('chart').getContext('2d');
      const chart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: [],
          datasets: [
            { label: 'Temp (°C)', borderColor: 'red', data: [], fill: false },
            { label: 'Humidity (%)', borderColor: 'blue', data: [], fill: false },
            { label: 'Gas (ppm)', borderColor: 'green', data: [], fill: false }
          ]
        },
        options: { responsive: true, scales: { x: { title: { display: true, text: 'Time (s)' } } } }
      });

      let counter = 0;
      async function updateData() {
        const res = await fetch("/data.json");
        const data = await res.json();

        document.getElementById('temp').innerText = data.temperature.toFixed(1);
        document.getElementById('humi').innerText = data.humidity.toFixed(1);
        document.getElementById('gas').innerText = data.mq9.toFixed(1);

        chart.data.labels.push(counter++);
        chart.data.datasets[0].data.push(data.temperature);
        chart.data.datasets[1].data.push(data.humidity);
        chart.data.datasets[2].data.push(data.mq9);

        if (chart.data.labels.length > 20) {
          chart.data.labels.shift();
          chart.data.datasets.forEach(ds => ds.data.shift());
        }
        chart.update();

        const alertBox = document.getElementById('alert');
        if (data.danger) {
          alertBox.innerText = "🚨 WARNING: Fire Risk Detected!";
          alertBox.className = "danger";
        } else {
          alertBox.innerText = "✅ System Safe";
          alertBox.className = "safe";
        }
      }

      setInterval(updateData, 2000);
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleDataJson()
{
  String data = "{";
  data += "\"temperature\":" + String(t, 1) + ",";
  data += "\"humidity\":" + String(h, 1) + ",";
  data += "\"mq9\":" + String(mq9_ppm, 1) + ",";
  data += "\"danger\":" + String(danger ? "true" : "false");
  data += "}";
  server.send(200, "application/json", data);
}

//////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_OK_GR, OUTPUT);
  pinMode(LED_NOTOK_RED, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_OK_GR, LOW);
  digitalWrite(LED_NOTOK_RED, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 Project");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data.json", handleDataJson);
  server.begin();
}

void loop()
{
  readSensors();
  updateLCD();
  server.handleClient();
  delay(2000);
}
