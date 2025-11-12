#include "Arm_Servo.h"
#include "MotorDriver.h"  // ✅ THÊM dòng này

Servo servo1, servo2;
#define Servo1Pin 9
#define Servo2Pin 11

int minAngle = 40;
int maxAngle = 180;
int minAngle2 = 80;
int stepDelay = 5;

// ✅ External reference to motor object
extern MotorDriver motor;  // Từ main.cpp

void ArmInit() {
    servo1.attach(Servo1Pin);
    servo2.attach(Servo2Pin);
}

void Nhiemvu1_5() {
    for (int angle = maxAngle; angle >= minAngle; angle--) {
        servo1.write(angle);
        delay(stepDelay);
    }
}

void Nhiemvu3() {
    for (int angle = maxAngle; angle >= minAngle2; angle--) {
        servo2.write(angle);
        delay(stepDelay);
    }
}

void Nhiemvu4_7() {
    servo1.write(65);

    motor.Speed(70, -73);
    motor.Distance(35);
    motor.Speed(0, 0);
}

void Reset_lentren() {
    for (int angle = minAngle; angle <= maxAngle; angle++) {
        servo1.write(angle);
        delay(stepDelay);
    }
}

void Reset_xuongduoi() {
    for (int angle = maxAngle; angle >= minAngle; angle--) {
        servo1.write(angle);
        delay(stepDelay);
    }
}
