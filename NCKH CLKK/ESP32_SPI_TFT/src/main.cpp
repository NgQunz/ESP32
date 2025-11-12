#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// --- Dữ liệu ---
float temp;
float humi;
float co;
float nh3;

// --- Ngưỡng ---
#define TEMP_HIGH 35.0
#define HUMI_LOW 30.0
#define HUMI_HIGH 70.0
#define CO_HIGH 50.0
#define NH3_HIGH 25.0

void randomData()
{
  temp = random(200, 400) / 10.0; // 20.0 - 40.0 °C
  humi = random(30, 90);
  co = random(10, 200);
  nh3 = random(5, 100);
}

int collectWarnings(String warnList[], int maxItems)
{
  int count = 0;
  if (temp > TEMP_HIGH && count < maxItems)
    warnList[count++] = "HIGH TEMP!";
  if ((humi < HUMI_LOW || humi > HUMI_HIGH) && count < maxItems)
    warnList[count++] = "ABN HUMI!";
  if (co > CO_HIGH && count < maxItems)
    warnList[count++] = "HIGH CO!";
  if (nh3 > NH3_HIGH && count < maxItems)
    warnList[count++] = "HIGH NH3!";
  return count;
}

void drawScreen()
{
  int w = tft.width();
  int h = tft.height();
  int boxW = w / 2;
  int boxH = h / 2;

  // Gom cảnh báo
  String warns[4];
  int warnCount = collectWarnings(warns, 4);
  bool warning = (warnCount > 0);

  // Nền
  uint16_t bg = warning ? TFT_RED : TFT_BLUE;
  tft.fillScreen(bg);

  // Kẻ khung 4 ô
  tft.drawLine(boxW, 0, boxW, h, TFT_WHITE); // dọc
  tft.drawLine(0, boxH, w, boxH, TFT_WHITE); // ngang
  tft.drawRect(0, 0, w, h, TFT_WHITE);       // viền ngoài

  // Hiển thị dữ liệu
  tft.setTextColor(TFT_WHITE, bg);
  tft.setTextSize(2);
  char buf[32];

  // Ô 1: Nhiệt độ + Độ ẩm
  sprintf(buf, "%.1f C", temp);
  tft.setCursor(20, 30);
  tft.print("TEMP ");
  tft.print(buf);

  sprintf(buf, "%.0f %%", humi);
  tft.setCursor(20, 80);
  tft.print("HUMI ");
  tft.print(buf);

  // Ô 2: Cảnh báo (trên phải)
  if (warning)
  {
    tft.setTextColor(TFT_YELLOW, bg);
    tft.setTextSize(2);
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
    tft.setTextSize(2);
    tft.print("OK");
  }

  // Ô 3: CO (dưới trái)
  sprintf(buf, "%.0f ppm", co);
  tft.setTextColor(TFT_WHITE, bg);
  tft.setCursor(20, boxH + 40);
  tft.print("CO");
  tft.setCursor(20, boxH + 65);
  tft.print(buf);

  // Ô 4: NH3 (dưới phải)
  sprintf(buf, "%.0f ppm", nh3);
  tft.setCursor(boxW + 20, boxH + 40);
  tft.print("NH3");
  tft.setCursor(boxW + 20, boxH + 65);
  tft.print(buf);
}

void setup()
{
  tft.init();
  tft.setRotation(1);
  randomSeed(analogRead(0));
}

void loop()
{
  randomData();
  drawScreen();
  delay(2000);
}
