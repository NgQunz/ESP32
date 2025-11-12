#ifndef MOTOR_CONTROL_BASIC_H
#define MOTOR_CONTROL_BASIC_H

#include <Arduino.h>
#include <SoftPWM.h>

#define ML_DIR 1
#define MR_DIR 0
#define ML_PWM 8
#define MR_PWM 6

void MotorInit();
void Speed(int left, int right);
void Distance(float cm);
void turn_Left(int degree);
void turn_Right(int degree);
void runWithDS(int distance);

#endif
