#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

extern int RightMM;
extern int FrontMM;
extern int LeftMM;

extern float yawDeg;

void initI2C();
void initMPU6050();
void calibrateGyroZ();
void resetYaw();
void updateYaw();

void setupTOFAddresses();
void readTof();

#endif