#ifndef __LCD_LIBRARY_H__
#define __LCD_LIBRARY_H__

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;  // Khai báo extern

void setup_lcd();
void dichMang();
void run();

#endif
