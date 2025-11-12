/*
 An example analogue clock using a TFT LCD screen to show the time
 use of some of the drawing commands with the library.

 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo.

 This sketch uses font 4 only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################

 Based on a sketch by Gilchrist 6/2/2014 1.0
 */
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// --- Dữ liệu cảm biến ---
float temp;
int heartRate;
int spo2;
int bp_sys;
int bp_dia;

// Ngưỡng cảnh báo
#define TEMP_HIGH 38.0
#define HR_HIGH 100
#define SPO2_LOW 95
#define SYS_HIGH 140
#define DIA_HIGH 90

// Sinh dữ liệu random
void randomData()
{
  temp = random(360, 410) / 10.0; // 36.0 - 41.0 °C
  heartRate = random(60, 130);    // 60 - 130 bpm
  spo2 = random(90, 100);         // 90 - 99 %
  bp_sys = random(100, 160);      // 100 - 159 mmHg
  bp_dia = random(60, 100);       // 60 - 99 mmHg
}

// Gom tất cả cảnh báo
int collectWarnings(String warnList[], int maxItems)
{
  int count = 0;
  if (temp > TEMP_HIGH && count < maxItems)
    warnList[count++] = "HIGH TEMP!";
  if (heartRate > HR_HIGH && count < maxItems)
    warnList[count++] = "HIGH HEART!";
  if (spo2 < SPO2_LOW && count < maxItems)
    warnList[count++] = "LOW SpO2!";
  if (bp_sys > SYS_HIGH && count < maxItems)
    warnList[count++] = "HIGH SYS!";
  if (bp_dia > DIA_HIGH && count < maxItems)
    warnList[count++] = "HIGH DIA!";
  return count;
}

void drawScreen()
{
  int w = tft.width();
  int h = tft.height();

  int topHeight = h / 3;
  int boxHeight = (h - topHeight) / 2;
  int boxWidth = w / 2;

  // Gom cảnh báo
  String warns[5];
  int warnCount = collectWarnings(warns, 5);
  bool warning = (warnCount > 0);

  // Chọn màu nền
  uint16_t bg = warning ? TFT_RED : TFT_BLUE;
  tft.fillScreen(bg);

  // Kẻ bảng chia ô
  tft.drawLine(0, topHeight, w, topHeight, TFT_WHITE);                         // ngang dưới Temp
  tft.drawLine(0, topHeight + boxHeight, w, topHeight + boxHeight, TFT_WHITE); // ngang giữa
  tft.drawLine(boxWidth, topHeight, boxWidth, h, TFT_WHITE);                   // dọc chia đôi

  char buf[32];

  // Ô Temp
  sprintf(buf, "%.1f C", temp);
  tft.setTextColor(TFT_WHITE, bg);
  tft.setTextSize(2);
  tft.setCursor(10, 30);
  tft.print("TEMP: ");
  tft.print(buf);

  // Hiện cảnh báo chi tiết bên phải ô Temp
  if (warning)
  {
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, bg);
    int x = w / 2 + 20; // góc phải ô Temp
    int y = 20;
    for (int i = 0; i < warnCount; i++)
    {
      tft.setCursor(x, y + i * 22);
      tft.print(warns[i]);
    }
  }

  // Heart
  tft.setTextColor(TFT_WHITE, bg);
  tft.setTextSize(2);
  tft.setCursor(10, topHeight + 10);
  tft.print("HEART");
  sprintf(buf, "%d bpm", heartRate);
  tft.setCursor(10, topHeight + 35);
  tft.print(buf);

  // SpO2
  sprintf(buf, "%d %%", spo2);
  tft.setCursor(boxWidth + 10, topHeight + 10);
  tft.print("SpO2");
  tft.setCursor(boxWidth + 10, topHeight + 35);
  tft.print(buf);

  // Sys
  tft.setCursor(10, topHeight + boxHeight + 10);
  tft.print("SYS");
  sprintf(buf, "%d mmHg", bp_sys);
  tft.setCursor(10, topHeight + boxHeight + 35);
  tft.print(buf);

  // Dia
  tft.setCursor(boxWidth + 10, topHeight + boxHeight + 10);
  tft.print("DIA");
  sprintf(buf, "%d mmHg", bp_dia);
  tft.setCursor(boxWidth + 10, topHeight + boxHeight + 35);
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
/*
 An example showing rainbow colours on a 2.2" TFT LCD screen
 and to show basic examples of font use.

 This sketch uses the GLCD, 2, 4, 6 fonts only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */

// #include <TFT_eSPI.h> // Hardware-specific library
// #include <SPI.h>

// TFT_eSPI tft = TFT_eSPI(); // Invoke custom library with default width and height

// // TFT_eSPI tft = TFT_eSPI(240, 320);       // Could invoke custom library declaring width and height

// unsigned long targetTime = 0;
// byte red = 31;
// byte green = 0;
// byte blue = 0;
// byte state = 0;
// unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each
// void rainbow_fill();
// void setup(void)
// {
//   Serial.begin(9600);
//   tft.init();
//   tft.setRotation(2);
//   tft.fillScreen(TFT_BLACK);

//   targetTime = millis() + 1000;
// }

// void loop()
// {

//   if (targetTime < millis())
//   {
//     targetTime = millis() + 10000;

//     rainbow_fill(); // Fill the screen with rainbow colours

//     // The standard AdaFruit font still works as before
//     tft.setTextColor(TFT_BLACK); // Background is not defined so it is transparent

//     tft.setCursor(60, 5);
//     tft.setTextFont(0); // Select font 0 which is the Adafruit font
//     tft.print("Original Adafruit font!");

//     // tft.drawString("Original Adafruit font!",60,5,1);

//     // The new larger fonts do not need to use the .setCursor call, coords are embedded
//     tft.setTextColor(TFT_BLACK); // Do not plot the background colour
//     // Overlay the black text on top of the rainbow plot (the advantage of not drawing the background colour!)
//     tft.drawCentreString("Font size 2", 120, 14, 2); // Draw text centre at position 120, 14 using font 2
//     tft.drawCentreString("Font size 4", 120, 30, 4); // Draw text centre at position 120, 30 using font 4
//     tft.drawCentreString("12.34", 120, 54, 6);       // Draw text centre at position 120, 54 using font 6

//     tft.drawCentreString("12.34 is in font size 6", 120, 92, 2); // Draw text centre at position 120, 92 using font 2
//     // Note the x position is the top of the font!

//     // draw a floating point number
//     float pi = 3.14159;                                     // Value to print
//     int precision = 3;                                      // Number of digits after decimal point
//     int xpos = 90;                                          // x position
//     int ypos = 110;                                         // y position
//     int font = 2;                                           // font number 2
//     xpos += tft.drawFloat(pi, precision, xpos, ypos, font); // Draw rounded number and return new xpos delta for next print position
//     tft.drawString(" is pi", xpos, ypos, font);             // Continue printing from new x position

//     tft.setTextSize(1); // We are using a size multiplier of 1

//     tft.setTextColor(TFT_BLACK); // Set text colour to black, no background (so transparent)

//     tft.setCursor(36, 150, 4);     // Set cursor to x = 36, y = 150 and use font 4
//     tft.println("Transparent..."); // As we use println, the cursor moves to the next line

//     tft.setCursor(30, 175);                 // Set cursor to x = 30, y = 175
//     tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text colour to white and background to black
//     tft.println("White on black");

//     tft.setTextFont(4);     // Select font 4 without moving cursor
//     tft.setCursor(50, 210); // Set cursor to x = 50, y = 210 without changing the font
//     tft.setTextColor(TFT_WHITE);
//     // By using #TFT print we can use all the formatting features like printing HEX
//     tft.print(57005, HEX);   // Cursor does no move to next line
//     tft.println(48879, HEX); // print and move cursor to next line

//     tft.setTextColor(TFT_GREEN, TFT_BLACK); // This time we will use green text on a black background
//     tft.setTextFont(2);                     // Select font 2
//     // Text will wrap to the next line if needed, by luck it breaks the lines at spaces!
//     tft.println(" Ode to a Small Lump of Green Putty I Found in My Armpit One Midsummer Morning ");
//   }
// }

// // Fill screen with a rainbow pattern
// void rainbow_fill()
// {
//   // The colours and state are not initialised so the start colour changes each time the function is called

//   for (int i = 319; i > 0; i--)
//   {
//     // Draw a vertical line 1 pixel wide in the selected colour
//     tft.drawFastHLine(0, i, tft.width(), colour); // in this example tft.width() returns the pixel width of the display
//     // This is a "state machine" that ramps up/down the colour brightnesses in sequence
//     switch (state)
//     {
//     case 0:
//       green++;
//       if (green == 64)
//       {
//         green = 63;
//         state = 1;
//       }
//       break;
//     case 1:
//       red--;
//       if (red == 255)
//       {
//         red = 0;
//         state = 2;
//       }
//       break;
//     case 2:
//       blue++;
//       if (blue == 32)
//       {
//         blue = 31;
//         state = 3;
//       }
//       break;
//     case 3:
//       green--;
//       if (green == 255)
//       {
//         green = 0;
//         state = 4;
//       }
//       break;
//     case 4:
//       red++;
//       if (red == 32)
//       {
//         red = 31;
//         state = 5;
//       }
//       break;
//     case 5:
//       blue--;
//       if (blue == 255)
//       {
//         blue = 0;
//         state = 0;
//       }
//       break;
//     }
//     colour = red << 11 | green << 5 | blue;
//   }
// }
