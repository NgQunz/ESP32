#include <Arduino.h>
#include <adc.h>

int led[]={4,16,17,18,19,22,33,23};
void setup()
{
  Serial.begin(9600);
  for (int k=0;k<8;k++)
  {
    pinMode(led[k],OUTPUT);
    digitalWrite(led[k],1);
  }
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
}
void loop(){
  chinh_do_sang();
}
