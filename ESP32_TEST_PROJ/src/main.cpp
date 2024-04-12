#include <header.h>

int count = 0;
hw_timer_t* Timer0_Cfg = NULL;
uint8_t flag_stt = 0;
void IRAM_ATTR MQTT()
{
  flag_stt = 1;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  SET_ALL();
  Wifi_Connect();
  MQTT_Connect();
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
}
void loop() 
{
  Flag_Status();
  if (flag.flag_start)
  {
    flag.flag_mqtt = 1; // mqtt
    flag.flag_start = 0;
    flag.flag_check_id = 1;
    flag.flag_delay = 1;
    flag.flag_Gas = 1;
    flag.flag_Temp = 1;
    flag.flag_che_do = 1;
    flag.flag_mqtt_control = 1;
    flag.flag_auto_all = 1;
    flag.flag_open_door = 1;
    flag.flag_but_ctrl = 1;
    flag.flag_but_read = 1;
    flag.flag_lcd = 1;
    flag.flag_led_e3f = 1;
    flag.flag_cb_as = 1;
    count++;
  }
  if (count==10)
  {
    flag.flag_open_door =1;
    count=0;
  }
  CHE_DO();
}
