#ifndef __VL53LX_H__
#define __VL53LX_H__

#include <Adafruit_VL53L0X.h>
#include <Arduino.h>
#include <Wire.h>

#define XSHUT1 25       // Chân XSHUT của cảm biến 1
#define XSHUT2 26       // Chân XSHUT của cảm biến 2
#define XSHUT3 27       // Chân XSHUT của cảm biến 3
#define SENSOR_LIMIT 50 // 50mm

void vl53lx_setup();
void vl53lx_run();

int wallFront();
int wallRight();
int wallLeft();

#endif