#include <Arduino.h>
int a[]={4,16,17,18,19,27,32,22};
hw_timer_t *Timer0_Cfg=NULL;
uint32_t Freq=0;
uint8_t flag_stt=0;
void IRAM_ATTR Blink_Led()
{
  flag_stt=!flag_stt;
}
void setup() {
  for (int i=0;i<8;i++)
  {
    pinMode(a[i], OUTPUT);
    digitalWrite(a[i],1);
  }
  Serial.begin(115200);
  Timer0_Cfg= timerBegin(0,8000,true);
  timerAttachInterrupt(Timer0_Cfg, &Blink_Led,true);
  timerAlarmWrite(Timer0_Cfg,10000,true);
  timerAlarmEnable(Timer0_Cfg);
}
void loop() {
  if(flag_stt)
  {
    for (int i=0;i<8;i++)
    {
      digitalWrite(a[i],0);
    }
  }
  else 
  {
    for (int i=0;i<8;i++)
    {
      digitalWrite(a[i],1);
    }
  }
  Serial.println("Value ADC: " + String(flag_stt));
}
