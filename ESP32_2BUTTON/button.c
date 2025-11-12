#include "button.h"

#define BUTTON_PIN 15
#define LED_PIN 2

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;
static int buttonState = HIGH;
static int lastReading = HIGH;

void setup_button()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
}

void run_button()
{
    int reading = digitalRead(BUTTON_PIN);

    if (reading != lastReading)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (reading != buttonState)
        {
            buttonState = reading;

            if (buttonState == LOW)
            {
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            }
        }
    }

    lastReading = reading;
}
