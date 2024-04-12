#include <header.h>

int count = 0;

hw_timer_t* Timer0_Cfg = NULL;
uint8_t flag_stt = 0;
void IRAM_ATTR MQTT()
{
  flag_stt = 1;
}
void loop() {
  Stay_Connect_MQTT();
  if (flag_stt)
  {
    count++;
    MQTT_Pub_Sub();
    flag_stt = 0;
  }
  if (count==50)
  {
    To_mqtt_browser();
    count=0;
  }
}
void setup() {
  Serial.begin(9600);
  Wifi_Connect();
  MQTT_Connect();
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
}