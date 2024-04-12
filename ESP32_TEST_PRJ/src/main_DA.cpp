#include <header_DA.h>

hw_timer_t *timer = NULL;
// Flag flag;

void IRAM_ATTR onTimer()
{
  flag.flag_start = 1;
}

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  RFID_Config();
  Servo_set();
  Wire.begin();
  LCD_set();
  set_pin_mode();
  DHT_set();

  timer = timerBegin(0, 8000, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);

  set_pin_mode();
  DHT_set();

  Wifi_Connect();
  MQTT_Connect();
}

void loop()
{
  Stay_Connect_MQTT();
  if (flag.flag_start)
  {
    flag.flag_stt = 1;
    flag.flag_check_id = 1;
    flag.flag_delay = 1;
    flag.flag_Attach_Fan_manual_livingroom = 1;
    flag.flag_Attach_Fan_manual_kitchen = 1;
    flag.flag_Attach_Light_manual_livingroom = 1;
    flag.flag_Attach_Light_manual_kitchen = 1;
    flag.flag_Check_Gas = 1;
    flag.flag_Temperature = 1;
    
    flag.flag_start = 0;
  }
  ESP_to_browser_and_else ();
  Check_ID();
  // Dark_Light_Livingroom();
  // Attach_Fan_manual_livingroom();
  Attach_Light_manual_livingroom();
  Attach_Light_manual_kitchen();
  // Check_Gas();
  // Temperature_and_Humidity();
  // Stay_Connect_MQTT();
}
