// LEDController.cpp
#include "LEDController.h"
#include "PinConfig.h"

void ledSteady() { digitalWrite(LED_PIN, HIGH); }
void ledOff() { digitalWrite(LED_PIN, LOW); }

void ledBlinkSlow(unsigned long duration)
{
    unsigned long startTime = millis();
    while (millis() - startTime < duration)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
}

void updateLED()
{
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle > 150)
    {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        lastToggle = millis();
    }
}
