#ifndef __MY_HEADER__
#define __MY_HEADER__

#include <Arduino.h>


#define IN1 26
#define IN2 27
#define IN3 22
#define IN4 23
#define ENA 25
#define ENB 21

void M1_up();
void Stop_M1();
void Stop_M2();
void M2_up();
void M1_down();
void M2_down();
void Speedup();
void Slowdown();
void L298_Init();

#endif