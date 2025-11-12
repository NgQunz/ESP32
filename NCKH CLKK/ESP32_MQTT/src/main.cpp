#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFiClientSecure.h>

// ==== WiFi ====
const char *ssid = "Open WorkShop 2.4GHz";
const char *password = "0000*0000";

// ==== HiveMQ Cloud ====
const char *mqtt_server = "ca79e77ad302449a8ecc4323f5a735dc.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "Nckh_clkk";
const char *mqtt_pass = "Nckh_clkk1";
const char *topic_pub = "sensor/data";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// ==== TFT ====
TFT_eSPI tft = TFT_eSPI();

// ==== DHT22 ====
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== MQ135 ====
#define MQ135_PIN 34
const float RL_135 = 10000.0;
const float VCC_135 = 5.0;
const float ADC_REF = 3.3;
const int ADC_MAX = 4095;
float R0_135 = 30000.0;
const float M_NH3 = -0.48;
const float B_NH3 = 0.35;
int samples = 10;

float readVoltage_MQ135()
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

float calcRs_MQ135(float vout)
{
  if (vout <= 0.0001)
    return 1e6;
  return RL_135 * (VCC_135 - vout) / vout;
}

float RsRo_to_ppm(float rs_ro, float m, float b)
{
  if (rs_ro <= 0)
    return -1;
  float log_ppm = (log10(rs_ro) - b) / m;
  return pow(10, log_ppm);
}

// ==== MQ7 ====
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

// ==== GP2Y1010AU0F ====
#define DUST_LED 13
#define DUST_ADC 33
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float readDust()
{
  float sumDust = 0;
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(DUST_LED, LOW);
    delayMicroseconds(samplingTime);
    float voMeasured = analogRead(DUST_ADC);
    delayMicroseconds(deltaTime);
    digitalWrite(DUST_LED, HIGH);
    delayMicroseconds(sleepTime);

    float voltage = voMeasured * (3.3 / 4095.0);
    float dustDensity = 0.17 * voltage - 0.1; // mg/m³
    if (dustDensity < 0)
      dustDensity = 0;

    sumDust += dustDensity;
    delay(500);
  }
  float avgDust_mg = sumDust / 10.0;      // mg/m³
  float avgDust_ug = avgDust_mg * 1000.0; // µg/m³

  Serial.print("Bụi trung bình: ");
  Serial.print(avgDust_mg, 3);
  Serial.print(" mg/m3 (");
  Serial.print(avgDust_ug, 1);
  Serial.println(" µg/m3)");

  return avgDust_ug; // trả về µg/m³ để hiển thị và gửi MQTT
}

// ==== Biến dữ liệu ====
float temp, humi, co_ppm, nh3_ppm, dust;

// ==== MQTT ====
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Đang kết nối MQTT...");
    if (client.connect("ESP32_Client", mqtt_user, mqtt_pass))
    {
      Serial.println("✅ Kết nối MQTT thành công!");
    }
    else
    {
      Serial.print("❌ Lỗi, mã: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void sendToMQTT()
{
  char payload[200];
  snprintf(payload, sizeof(payload),
           "{\"temp\":%.2f,\"humi\":%.2f,\"co\":%.2f,\"nh3\":%.2f,\"dust\":%.2f}",
           temp, humi, co_ppm, nh3_ppm, dust);
  client.publish(topic_pub, payload);
  Serial.println("📤 Đã gửi MQTT:");
  Serial.println(payload);
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

// ==== Setup ====
void setup()
{
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);

  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi OK");

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  analogSetPinAttenuation(MQ135_PIN, ADC_11db);
  analogSetPinAttenuation(MQ7_PIN, ADC_11db);
  analogSetPinAttenuation(DUST_ADC, ADC_11db);
  pinMode(DUST_LED, OUTPUT);
  dht.begin();
}

// ==== Loop ====
void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();

  humi = dht.readHumidity();
  temp = dht.readTemperature();

  float vout135 = readVoltage_MQ135();
  float Rs135 = calcRs_MQ135(vout135);
  float ratio135 = Rs135 / R0_135;
  nh3_ppm = RsRo_to_ppm(ratio135, M_NH3, B_NH3);

  int adc7 = analogRead(MQ7_PIN);
  float Rs7 = getMQ7Resistance(adc7);
  co_ppm = getCOppm(Rs7);

  dust = readDust();

  drawScreen();
  sendToMQTT();

  delay(5000);
}
