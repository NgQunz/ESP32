#include "Arm_Servo.h"

Servo servo1, servo2;

int minAngle = 40;
int maxAngle = 180;
int minAngle2 = 80;
int stepDelay = 5;

void ArmInit()
{
    servo1.attach(Servo1Pin);
    servo2.attach(Servo2Pin);
}

void Nhiemvu1_5()
{
    for (int angle = maxAngle; angle >= minAngle; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
}

void Nhiemvu3()
{
    servo2.write(maxAngle);
    delay(5000);
    for (int angle = maxAngle; angle >= minAngle2; angle--)
    {
        servo2.write(angle);
        delay(stepDelay);
    }
}

void Nhiemvu4_7()
{
    servo1.write(65);
    Speed(70, -73);
    Distance(35);
    Speed(0, 0);
}

void Reset_lentren()
{
    for (int angle = minAngle; angle <= maxAngle; angle++)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
}

void Reset_xuongduoi()
{
    for (int angle = maxAngle; angle >= minAngle; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
}