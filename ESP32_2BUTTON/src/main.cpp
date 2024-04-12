#include <Arduino.h>
#include <led.h>
// int count=0,k=0;
// #define PushButton_B 21
// #define PushButton_T 25
// int Push_button_state=0, led[]={4,16,17,18,19,33,22,23};
int value_ADC=0,Sum=0;
void setup() 
{
  Serial.begin(9600);
  ledcSetup(ledChannel, freq, resolution);
  pinMode(PushButton_B,INPUT);
  pinMode(PushButton_T,INPUT);
  for (int i=0;i<8;i++)
    {
      digitalWrite(led[i],1);
      pinMode(led[i],OUTPUT);
      ledcAttachPin(16, ledChannel);
    }
  Serial.begin(9600);
}
void loop() 
{
  shiftOut()
}
bool is_prime(int number) {
  if (number <= 1) 
  {
    return false;
  }
  if (number ==2)
  {
    return true;
  }
  if (number >2)
  {
    if (number%2==0)
    {
      return false;
    }
    else
    {
      for (int i = 3; i * i <= number; i+=2) 
      {
        if (number % i == 0) 
        {
          return false;
        }
      }
    }
  }
  return true;
}