// JunctionDetector.cpp
#include "JunctionDetector.h"
#include "RobotState.h"

unsigned long lastJunctionTime = 0;
const unsigned long JUNCTION_COOLDOWN = 1000;

void checkJunction(IRSensorArray &sensor, BuzzerController &buzzer)
{
    if (currentState != STATE_RUNNING)
        return;
    byte s = sensor.getDigitalByte();
    bool isJunction = (s == 0b11111) || (s == 0b00111) || (s == 0b11100);
    if (isJunction && (millis() - lastJunctionTime > JUNCTION_COOLDOWN))
    {
        Serial.println(F("T-JUNCTION DETECTED"));
        buzzer.beep(2500, 500);
        lastJunctionTime = millis();
    }
}
