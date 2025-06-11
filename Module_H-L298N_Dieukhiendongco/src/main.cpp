#include "l298.h"

int i;
void setup()
{
  L298_Init();
}

void loop()
{
  digitalWrite(IN3, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENB, 200);
  analogWrite(ENA, 200);
}

void L298_Init()
{
  // Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}
void M1_up()
{

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}
void Stop_M1()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}
void Stop_M2()
{
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void M2_up()
{

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void M1_down()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}
void M2_down()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}
void Speedup()
{
  for (i = 0; i < 255; i++)
  {
    analogWrite(ENA, i);
    M1_up();
    // digitalWrite(ENB, i);
    // M2_up();
    // delay(50);
  }
}
void Slowdown()
{
  for (i = 255; i > 0; i--)
  {
    analogWrite(ENA, i);
    // digitalWrite(IN1, HIGH);
    // digitalWrite(IN2, LOW);
    // digitalWrite(ENB, i);
    // digitalWrite(IN3, HIGH);
    // digitalWrite(IN4, LOW);
    M1_up();
    delay(50);
  }
}
