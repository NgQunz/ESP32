// const int rs = 14, en = 13, d4 = 27, d5 = 26, d6 = 25 , d7 = 33;

// void setup() {
//   pinMode(rs, OUTPUT);
//   pinMode(en, OUTPUT);
//   pinMode(d4, OUTPUT);
//   pinMode(d5, OUTPUT);
//   pinMode(d6, OUTPUT);
//   pinMode(d7, OUTPUT);

//   delay(50);
//   lcdCommand(0x33); // khởi động lcd
//   lcdCommand(0x32); // đặt ở chế độ 4 bit
//   lcdCommand(0x28); //đặt chế độ có 2 line
//   lcdCommand(0x0C); // tắt con trỏ
//   lcdCommand(0x01); // Xoá màn hình
//   delayMicroseconds(2000);
//   delayMicroseconds(2000);
// }

// void lcdCommand(byte command) {
//   digitalWrite(rs, LOW);
//   lcdPulse(command);
// }

// void lcdPrint(const char* text) {
//   digitalWrite(rs, HIGH);
//   for (int i = 0; text[i] != 0; i++) {
//     lcdPulse(text[i]);
//   }
// }

// void lcdPulse(byte data) {
//   digitalWrite(d4, (data >> 4) & 0x01);
//   digitalWrite(d5, (data >> 5) & 0x01);
//   digitalWrite(d6, (data >> 6) & 0x01);
//   digitalWrite(d7, (data >> 7) & 0x01);
//   digitalWrite(en, HIGH);
//   delayMicroseconds(1);
//   digitalWrite(en, LOW);
//   delayMicroseconds(1);

//   digitalWrite(d4, data & 0x01);
//   digitalWrite(d5, (data >> 1) & 0x01);
//   digitalWrite(d6, (data >> 2) & 0x01);
//   digitalWrite(d7, (data >> 3) & 0x01);
//   digitalWrite(en, HIGH);
//   delayMicroseconds(1);
//   digitalWrite(en, LOW);
//   delayMicroseconds(50);
// }

// void loop() {
//   lcdCommand(0x80); // cho con trỏ ở dòng 1
//   lcdPrint("CLB Dien Tu ");
//   lcdCommand(0xC0);//cho con tro ở dòng 2
//   lcdPrint("EC PTIT");
//   }
//esp32_lcd
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

const int rs = 16, en = 17, d4 = 18, d5 = 19, d6 = 21, d7 = 22;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
}

void loop() 
{
// hien chu hello trong 2 giay
  lcd.setCursor(5, 0);
  lcd.print("Hello");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("World");
  delay(2000);
  lcd.clear();  
// Tắt màn hình di trong 2 giay
  lcd.noDisplay();  
  delay(2000);
// Hiển thị màn hình lại
  lcd.display();
// Chờ 2 giây
  delay(2000); 
}
