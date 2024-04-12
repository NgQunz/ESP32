#include "header.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void Start_Lcd()
{ 
    lcd.init();            
    lcd.backlight();
}

void Print_Temp()
{
    if ( count_timer % 20 == 0) // 2s lấy nhiệt độ 1 lần
    {
      Temp();
    }
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    if ( t < 10 && t > -10)
    {
      lcd.print(" ");
    }
    lcd.print(t);
    lcd.print("C  ");
    
}

void Print_Gas()
{
    if ( count_timer % 10 == 0) // 1s lấy nồng độ gas 1 lần
    {
      Gas();
    }
    lcd.setCursor(0, 1);
    lcd.print("Gas:");
    if ( gas < 10 )
    {
      lcd.print(" 0");
    }
    else if ( gas >= 10 && gas <= 99)
    {
      lcd.print(" ");
    }
    lcd.print(gas);
    lcd.print("%");
}

void Print_Funtion()
{
  lcd.setCursor(10, 1);
  if ( flag_auto == 1)
  {
    lcd.print("  AUTO");
  }
  else if ( flag_auto == 0)
  {
    lcd.print("MANUAL");
  }
}