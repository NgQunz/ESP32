N#include <Arduino.h>
#define freq  5000 // tần số xung
int resolution =8; // độ phân giải 8bit
int value_ADC=0,Sum=0;
#define ledPin 16
#define PIN_VR 33
#define ledChannel 0
void chinh_do_sang()
{
    for(int i = 0; i < 10; i++){
    value_ADC = analogRead(PIN_VR);
    Sum += value_ADC;
    delay(50);
  }
  Sum/=10;
  int dutyCycle=map(Sum,0,4095,0,255);
    ledcWrite(ledChannel, dutyCycle);
    Serial.println("Value ADC: " + String(dutyCycle));
    delay(1);
}
void bat_den(int* led){
  for(int i = 0; i < 10; i++){
    value_ADC = analogRead(PIN_VR);
    Sum += value_ADC;
    delay(50);
  }
  Sum/=10;
  int kq=map(Sum,0,4095,0,5000);
  kq/=625;
  Serial.println("Value ADC: " + String(kq));
  for (int i=0;i<kq;i++)
  {
    digitalWrite(led[i],0);
    if (i<=kq-1)
    {
      for (int j=7;j>i;j--)
      digitalWrite(led[j],1);
    }
  }
  if (kq==0)
    {
      digitalWrite(led[0],1);
    }
}