// CalibrationManager.cpp
#include "CalibrationManager.h"
#include "LEDController.h"
#include "PinConfig.h"
void calibrateWhiteWithLED(IRSensorArray &sensor)
{
    Serial.println(F("\n=== CALIB TRANG ==="));
    ledBlinkSlow(2000);
    const int samples = 50;
    int rawValues[NUM_SENSORS];
    for (int i = 0; i < NUM_SENSORS; i++)
        sensor._whiteValues[i] = 0;
    for (int s = 0; s < samples; s++)
    {
        sensor.readRaw(rawValues);
        for (int i = 0; i < NUM_SENSORS; i++)
            sensor._whiteValues[i] += rawValues[i];
        digitalWrite(LED_PIN, s % 2);
        delay(10);
    }
    for (int i = 0; i < NUM_SENSORS; i++)
        sensor._whiteValues[i] /= samples;
    ledSteady();
    Serial.println(F("✓ Calib trắng xong"));
}

void calibrateBlackWithLED(IRSensorArray &sensor)
{
    Serial.println(F("\n=== CALIB ĐEN ==="));
    ledBlinkSlow(2000);
    const int samples = 50;
    int rawValues[NUM_SENSORS];
    for (int i = 0; i < NUM_SENSORS; i++)
        sensor._blackValues[i] = 0;
    for (int s = 0; s < samples; s++)
    {
        sensor.readRaw(rawValues);
        for (int i = 0; i < NUM_SENSORS; i++)
            sensor._blackValues[i] += rawValues[i];
        digitalWrite(LED_PIN, s % 2);
        delay(10);
    }
    for (int i = 0; i < NUM_SENSORS; i++)
        sensor._blackValues[i] /= samples;
    ledSteady();
    Serial.println(F("✓ Calib đen xong"));
}
