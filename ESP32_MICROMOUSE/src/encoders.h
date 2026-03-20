#ifndef ENCODERS_H
#define ENCODERS_H

#include <Arduino.h>

extern volatile long encoder1Count;
extern volatile long encoder2Count;

void initEncoders();
void resetEncoders();
long avgAbsEncoder();
float encoderToMM(long pulse);

#endif