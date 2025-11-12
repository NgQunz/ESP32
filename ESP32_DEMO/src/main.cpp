#include <Arduino.h>
void setup()
{
    pinMode(5, OUTPUT);
}
void loop()
{
    digitalWrite(5, 1);
    delay(1000);
    digitalWrite(5, 0);
    delay(1000);
}
