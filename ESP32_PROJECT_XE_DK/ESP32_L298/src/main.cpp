#include <Arduino.h>

#define IN1 26
#define IN2 27
#define ENA 4
#define IN3 21
#define IN4 22
#define ENB 13
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
  analogWrite(ENA, 100);
  digitalWrite (IN1, LOW);// tiến
  digitalWrite (IN2, HIGH);
  analogWrite(ENB, 100);
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
struct JoystickData {
  int x_Pick_Up;
  int y_Pick_Up;
  int switchState_Pick_Up;
  int x_Control_Motor;
  int y_Control_Motor;
  int Poten;
};
JoystickData receivedData;
void RUN_FWD() 
{
  // if (flag_run_fwd==1)
  // {
    JoystickData receivedData;
    if (y2<20&&y2>-20)
    {
      // if (x2>20&&flag.flag_cs100a==1)
      if (x2>20)
      {
        analogWrite(ENA, x2);
        digitalWrite (IN1, LOW);// tiến
        digitalWrite (IN2, HIGH);
        analogWrite(ENB, x2);
        digitalWrite (IN3, LOW);// tiến
        digitalWrite (IN4, HIGH);
      }
      if (x2<-20)
      {
        analogWrite(ENA, -x2);
        digitalWrite (IN2, LOW);// lùi
        digitalWrite (IN1, HIGH);
        analogWrite(ENB, -x2);
        digitalWrite (IN3, HIGH);// lùi
        digitalWrite (IN4, LOW);
      }
      if (x2>-20&&x2<20)
      {
        Motor_off();
      }
    }
    if (y2>20||y2<-20)
    {
      if (y2>20)
      {
        // if (x2>20&&flag.flag_cs100a==1)
        if (x2>20) // rẽ phải
        {
          analogWrite(ENA, y2);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, x2);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        // if (x2<-20&&flag.flag_cs100a==1)
        if (x2<-20) // rẽ trái
        {
          analogWrite(ENA, -x2);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, y2);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
      }
      if (y2<-20)
      {
        if (x2>20) // rẽ phải lùi
        {
          analogWrite(ENA, x2);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, -y2);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
        if (x2<-20) // rẽ trái lùi - lùi 1 tiến 2
        {
          analogWrite(ENA, -y2);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, -x2);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
      }
      if (x2>-20&&x2<20)
      {
        if (y2>20)// tiến 2 tắt 1
        {
          analogWrite(ENB, y2);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        if (x2<-20) // tiến 1 tắt 2
        {
          analogWrite(ENA, -x2);
          digitalWrite (IN1, HIGH);
          digitalWrite (IN2, LOW);// tiến

        }
      }
    }
  // }
}
void Motor_off()
{
  digitalWrite (IN2, LOW);// lùi
  digitalWrite (IN1, LOW);
  digitalWrite (IN3, LOW);// lùi
  digitalWrite (IN4, LOW);
}