#include <lcd_library.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Khởi tạo LCD

char a[] = {'E', 'C', ' ', 'C', 'H', 'A', 'L', 'L', 'E', 'N', 'G', 'E', ' ', ' ', ' ', ' '};

void setup_lcd() {
  Wire.begin(21, 22);  // Khởi tạo I2C với SDA = 21, SCL = 22
  lcd.init();         // Khởi động LCD
  lcd.backlight();    // Bật đèn nền LCD
}

void dichMang() {
  for (int i = 15; i > 0; i--) {
    a[i] = a[i - 1];
  }
  a[0] = a[15];
}

void run() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(a);
  delay(500);
  dichMang();
}
