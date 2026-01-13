#include "Arm_Servo.h"
#include "MotorDriver.h" // ✅ THÊM dòng này
#include "PIDLineFollower.h"
Servo servo1, servo2;

int minAngle = 40;
int maxAngle = 180;
int minAngle2 = 80;
int stepDelay = 5;

// ✅ External reference to motor object
extern MotorDriver motor; // Từ main.cpp

void ArmInit()
{
    servo1.attach(Servo1Pin);
    servo2.attach(Servo2Pin);
    servo1.write(maxAngle);
    servo2.write(maxAngle);
    delay(1000);
}

void Nhiemvu1()
{
    // Lùi lại
    Serial.println(F("→ Reversing"));

    motor.move(-40, -70, 460);
    motor.stop();
    delay(200);
    motor.move(60, -60, 260);
    motor.stop();
    delay(300);
    for (int angle = maxAngle; angle >= minAngle; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
    delay(300);
    motor.move(-60, 60, 300);
    motor.stop();
    delay(300);
}

void Nhiemvu5()
{
    // Lùi lại
    Serial.println(F("→ Reversing"));

    motor.move(-60, 60, 200);
    motor.stop();
    delay(300);
    for (int angle = maxAngle; angle >= minAngle; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
    delay(300);
    motor.move(-60, 60, 300);
    motor.stop();
    delay(300);
}

void Nhiemvu6()
{

    for (int angle = maxAngle; angle >= 75; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }
    delay(300);
    motor.move(120, -120, 700);
    motor.stop();
    delay(1000);
    motor.move(120, -120, 600);
    motor.stop();
    delay(300);
}
void Nhiemvu3()
{

    motor.move(-60, 60, 200);
    motor.stop();
    delay(300);
    motor.move(40, 60, 180);
    delay(200);
    motor.stop();
    for (int angle = maxAngle; angle >= minAngle2; angle--)
    {
        servo2.write(angle);
        delay(stepDelay);
    }
    motor.move(60, -60, 200);
    motor.stop();
    delay(300);
    motor.move(-40, -60, 800);
    delay(200);
    motor.stop();
}

void Nhiemvu4()
{
    motor.move(-40, -60, 400);
    delay(200);
    motor.stop();
    for (int angle = maxAngle; angle >= 100; angle--)
    {
        servo1.write(angle);
        delay(stepDelay);
    }

    motor.move(-40, -60, 800);
    delay(200);
    motor.stop();
}
void Nhiemvu4_7()
{
    servo1.write(65);

    motor.Speed(70, -73);
    motor.Distance(35);
    motor.Speed(0, 0);
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
