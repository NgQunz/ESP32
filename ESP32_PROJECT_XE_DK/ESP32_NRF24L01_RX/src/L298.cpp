#include <header.h>

void L298N_SETUP() 
{
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
    JoystickData receivedData;
    //Serial.printf("\nOKKKKK\n");
    if (receivedData.y_Control_Motor<20&&receivedData.y_Control_Motor>-20)
    {
      // if (receivedData.x_Control_Motor>20&&flag.flag_cs100a==1)
      if (receivedData.x_Control_Motor>20)
      {
        Serial.printf("\nOKKKKK\n");
        analogWrite(ENA, receivedData.x_Control_Motor);
        digitalWrite (IN1, LOW);// tiến
        digitalWrite (IN2, HIGH);
        analogWrite(ENB, receivedData.x_Control_Motor);
        digitalWrite (IN3, LOW);// tiến
        digitalWrite (IN4, HIGH);
      }
      if (receivedData.x_Control_Motor<-20)
      {
        analogWrite(ENA, -receivedData.x_Control_Motor);
        digitalWrite (IN2, LOW);// lùi
        digitalWrite (IN1, HIGH);
        analogWrite(ENB, -receivedData.x_Control_Motor);
        digitalWrite (IN3, HIGH);// lùi
        digitalWrite (IN4, LOW);
      }
      if (receivedData.x_Control_Motor>-20&&receivedData.x_Control_Motor<20)
      {
        Motor_off();
      }
    }
    if (receivedData.y_Control_Motor>20||receivedData.y_Control_Motor<-20)
    {
      if (receivedData.y_Control_Motor>20)
      {
        // if (receivedData.x_Control_Motor>20&&flag.flag_cs100a==1)
        if (receivedData.x_Control_Motor>20) // rẽ phải
        {
          analogWrite(ENA, receivedData.y_Control_Motor);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, receivedData.x_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        // if (receivedData.x_Control_Motor<-20&&flag.flag_cs100a==1)
        if (receivedData.x_Control_Motor<-20) // rẽ trái
        {
          analogWrite(ENA, -receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, receivedData.y_Control_Motor);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
      }
      if (receivedData.y_Control_Motor<-20)
      {
        if (receivedData.x_Control_Motor>20) // rẽ phải lùi
        {
          analogWrite(ENA, receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);// tiến
          digitalWrite (IN2, LOW);
          analogWrite(ENB, -receivedData.y_Control_Motor);
          digitalWrite (IN3, LOW);// lùi
          digitalWrite (IN4, HIGH);
        }
        if (receivedData.x_Control_Motor<-20) // rẽ trái lùi - lùi 1 tiến 2
        {
          analogWrite(ENA, -receivedData.y_Control_Motor);
          digitalWrite (IN1, LOW);// lùi
          digitalWrite (IN2, HIGH);
          analogWrite(ENB, -receivedData.x_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
      }
      if (receivedData.x_Control_Motor>-20&&receivedData.x_Control_Motor<20)
      {
        if (receivedData.y_Control_Motor>20)// tiến 2 tắt 1
        {
          analogWrite(ENB, receivedData.y_Control_Motor);
          digitalWrite (IN3, HIGH);// tiến
          digitalWrite (IN4, LOW);
        }
        if (receivedData.x_Control_Motor<-20) // tiến 1 tắt 2
        {
          analogWrite(ENA, -receivedData.x_Control_Motor);
          digitalWrite (IN1, HIGH);
          digitalWrite (IN2, LOW);// tiến

        }
      }
    }
    //flag_motor=0;
  }
}
void Motor_off()
{
  digitalWrite (IN2, LOW);// lùi
  digitalWrite (IN1, LOW);
  digitalWrite (IN3, LOW);// lùi
  digitalWrite (IN4, LOW);
}