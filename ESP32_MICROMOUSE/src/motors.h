#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

void initMotors();
void setMotor1(int pwm);
void setMotor2(int pwm);
void setMotor(int leftPWM, int rightPWM);
void stopAllMotors();
void brakeStop();

#endif