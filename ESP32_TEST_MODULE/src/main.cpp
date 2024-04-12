#include "header.h"

void setup() 
{
  Serial.begin(115200);
  Setup_Timer();
  Start_RFID ();
  Declare_Door();
  Start_Lcd();
  Declare_Button();
  Declare_Sensor();
  Declare_Device();
  Setup_Wifi();
  MQTT_Connect();
}

void loop() 
{
  bool bitRead1(unsigned char value, int bit) { // đặt tên khác tránh trùng với thư viện có sẵn
    return ((value >> bit) & 1); // thêm & 1 để trả về 0 và 1 0&1 = 0, 1&1=1
  }
  bitRead(0,0);
  // Print_Temp();
  // Print_Gas();
  // Manual_Button();
  
  // if ( Press_Button35() == 1)
  // {
  //     flag_auto = 1;
  // }
  // if ( flag_auto == 1)
  // {
  //   Kitchen_Auto();
  //   Living_Auto();
  // }
  // if ( flag_pause == 1)
  // {
  //   Check_RFID();
  //   Wrong_RFID();
  //   Door_Light();
  //   MQTT_Sub();
  //   MQTT_Pub_Sub(); 
  //   if ( flag_open_door == 1)
  //   {
  //     Open_Door();
  //   }
  //   flag_pause = 0;
  // }

  // Client_Loop();
  // Print_Funtion();
  // Solve_Device();
  Temp();
}
