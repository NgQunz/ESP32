#include <Arduino.h>
#define PushButton_B 21
#define PushButton_T 25
int count=0;
int k=1, led[]={4,16,17,18,19,33,22,23};
const int freq = 5000;  // tần số xung
const int ledChannel = 3; // kênh PWM
const int resolution = 8; // độ phân giải 8bit
void bat_tat() 
{
    if (digitalRead(PushButton_B)==0)
    {
      for (int i=0;i<8;i++)
      {
        digitalWrite(led[i],0);
        delay(1);
      }
    }
    
    if (digitalRead(PushButton_T)==0)
    {
      for (int i=0;i<8;i++)
      {
        digitalWrite(led[i],1);
        delay(1);
      }
    }
}
void nhan_giu()
{
    if (digitalRead(PushButton_B)==0)
    {
      for (int i=0;i<8;i++)
      {
        digitalWrite(led[i],0);
      }
      delay(1);
      count++;
    }
    if (count==2000)
    {
      while (1)
      {
        for (int i=0;i<8;i++)
        {
          digitalWrite(led[i],0);
        }
        delay(200);
        for (int i=0;i<8;i++)
        {
          digitalWrite(led[i],1);
        }
        delay(200);
      }
      if (digitalRead(PushButton_B)==0)
      {
        for (int i=0;i<8;i++)
        {
          digitalWrite(led[i],1);
        }
        delay(1);
        count=0;
      }
    }
    if (digitalRead(PushButton_B)==0)
    {
      for (int i=0;i<8;i++)
      {
        digitalWrite(led[i],1);
      }
      delay(1);
    }
}
int buttonPushCounter = 0;   // số lần button được nhấn
int buttonState = 0;         // trạng thái hiện tại của button
int lastButtonState = 0;     // trạng thái trước đó của button
void chong_nhieu() 
{
  buttonState = digitalRead(PushButton_B);
  if (buttonState != lastButtonState) {
    if (buttonState == 0) 
    {
      buttonPushCounter++;
      Serial.println(buttonPushCounter);
    }
    else {
      Serial.println("off");
    }
  }
  lastButtonState = buttonState;
  if (buttonPushCounter % 4 == 0) { 
    for (int i=0;i<8;i++)
        {
          digitalWrite(led[i],0);
        }
  } else {
   for (int i=0;i<8;i++)
      {
        digitalWrite(led[i],1);
      }
  }
 
}