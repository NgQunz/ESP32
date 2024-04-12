#include <Arduino.h>
#define a 4
#define PIN_CB 13
hw_timer_t *Timer0_Cfg=NULL;
uint32_t Freq=0;
uint8_t flag_stt=0;
void IRAM_ATTR Blink_Led()
{
  flag_stt=!flag_stt;
}
void setup() {

    pinMode(a, OUTPUT);
    pinMode(PIN_CB, INPUT);
    digitalWrite(a,1);
  Timer0_Cfg= timerBegin(0,8000,true);
  timerAttachInterrupt(Timer0_Cfg, &Blink_Led,true);
  timerAlarmWrite(Timer0_Cfg,1000,true);
  timerAlarmEnable(Timer0_Cfg);
}
void loop() {
  int value=0;
  if(flag_stt)
  {
    int value=digitalRead(PIN_CB);
      if (value)
      {
        digitalWrite(a,0);
      }
      else 
      {
        digitalWrite(a,1);
      }
      Serial.println("Value ADC: " + String(flag_stt));

  }
}
