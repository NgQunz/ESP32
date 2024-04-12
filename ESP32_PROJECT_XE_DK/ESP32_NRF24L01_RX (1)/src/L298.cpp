#include <header.h>
JoystickData receivedData;
void L298N_SETUP() 
{
  //Serial.begin(9600);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (ENB, OUTPUT);
}

void RUN_FWD() 
{
  if (flag_motor==1)
  {
    if (receivedData.y_Control_Motor<120&&receivedData.y_Control_Motor>80)
    {
      if (receivedData.x_Control_Motor>-80&&flag_barrier==0)
      // if (receivedData.x_Control_Motor>20)
      {
        Serial.printf("\nOKKKKK\n");
        analogWrite(ENA, receivedData.x_Control_Motor);
        digitalWrite (IN1, LOW);// tiến
        digitalWrite (IN2, HIGH);
        analogWrite(ENB, receivedData.x_Control_Motor);
        digitalWrite (IN3, LOW);// tiến
        digitalWrite (IN4, HIGH);
      }
      if (receivedData.x_Control_Motor<-110)
      {
        analogWrite(ENA, -receivedData.x_Control_Motor);
        digitalWrite (IN2, LOW);// lùi
        digitalWrite (IN1, HIGH);
        analogWrite(ENB, -receivedData.x_Control_Motor);
        digitalWrite (IN3, HIGH);// lùi
        digitalWrite (IN4, LOW);
      }
      if (receivedData.x_Control_Motor>-110&&receivedData.x_Control_Motor<-80)
      {
        Motor_off();
      }
    }
    if (receivedData.y_Control_Motor>120||receivedData.y_Control_Motor<80)
    {
      if (receivedData.y_Control_Motor>120)
      {
        if (receivedData.x_Control_Motor>-80&&flag_barrier==0)
        // if (receivedData.x_Control_Motor>20) // rẽ phải
        {
          analogWrite(ENA, receivedData.y_Control_Motor);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, receivedData.x_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        if (receivedData.x_Control_Motor<-110&&flag_barrier==0)
        // if (receivedData.x_Control_Motor<-20) // rẽ trái
        {
          analogWrite(ENA, -receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, receivedData.y_Control_Motor);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
      }
      if (receivedData.y_Control_Motor<80)
      {
        if (receivedData.x_Control_Motor>-80) // rẽ phải lùi
        {
          analogWrite(ENA, receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, -receivedData.y_Control_Motor);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
        if (receivedData.x_Control_Motor<-110) // rẽ trái lùi - lùi 1 tiến 2
        {
          analogWrite(ENA, -receivedData.y_Control_Motor);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, -receivedData.x_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
      }
      if (receivedData.x_Control_Motor>-110&&receivedData.x_Control_Motor<-80) // xoay tròn
      {
        if (receivedData.y_Control_Motor>120)// tiến 2 tắt 1
        {
          analogWrite(ENB, receivedData.y_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        if (receivedData.y_Control_Motor<80) // tiến 1 tắt 2
        {
          analogWrite(ENA, -receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);
          digitalWrite (IN2, LOW);// tiến
        }
      }
    }
    flag_motor=0;
  }
}
void Motor_off()
{
  digitalWrite (IN2, LOW);// lùi
  digitalWrite (IN1, LOW);
  digitalWrite (IN3, LOW);// lùi
  digitalWrite (IN4, LOW);
}