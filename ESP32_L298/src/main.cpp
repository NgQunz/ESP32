#include <Arduino.h>

#define IN1 26
#define IN2 27
#define IN3 22
#define IN4 23
#define ENA 25
#define ENB 21
// 5 16 17 18 19 23 // nrf
// 34 35 // cs 100a
// 25 32 33 17
int x2,y2;
int flag_run_fwd=1;
void L298N_SETUP() ;
void RUN_FWD() ;
void Motor_off();
void setup()
{
  L298N_SETUP();
}
void loop()
{
  //RUN_FWD();
  analogWrite(ENA, 200);
  digitalWrite (IN1, LOW);// tiến
  digitalWrite (IN2, HIGH);
  analogWrite(ENB, 200);
  digitalWrite (IN3, LOW);// tiến
  digitalWrite (IN4, HIGH);
}
void L298N_SETUP() 
{
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (ENB, OUTPUT);
}