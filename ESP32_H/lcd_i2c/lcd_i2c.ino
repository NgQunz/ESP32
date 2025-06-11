#include <lcd.h>
static int num;
void setup()
{
  GPIO_Config();
}
void loop ()
{
  static num;
  led_Blink();
  Serial.println(num);
}