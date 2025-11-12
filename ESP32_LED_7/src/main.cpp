#include <Arduino.h>
int LED[7] = {17, 16, 13, 21, 22, 18, 19};
uint8_t val[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010,
                 0b01100110, 0b10110110, 0b10111110, 0b11100000,
                 0b11111110, 0b11110110};
void setup()
{
  for (int i = 0; i < 7; i++)
  {
    pinMode(LED[i], OUTPUT);
  }
}
void loop()
{
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      digitalWrite(LED[j], (val[i] >> (7 - j)) & 0x01);
    }
    delay(500);
  }
}
