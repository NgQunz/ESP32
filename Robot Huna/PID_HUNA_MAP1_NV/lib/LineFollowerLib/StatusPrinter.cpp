// StatusPrinter.cpp
#include "StatusPrinter.h"

void printStatus(PIDLineFollower &pid)
{
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint < 200)
        return;
    lastPrint = millis();
    Serial.print(F("Pos: "));
    Serial.print(pid.getLinePosition());
    Serial.print(F(" | Err: "));
    Serial.print(pid.getError());
    Serial.print(F(" | Corr: "));
    Serial.println(pid.getCorrection());
}
