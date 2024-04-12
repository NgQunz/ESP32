#include <header_DA.h>


void setup() {
  Serial.begin(9600);
  Wifi_Connect();
  MQTT_Connect();

}

void loop() {
  Stay_Connect_MQTT();
  if (flag.flag_stt)
  {
    cnt_mqtt++;
    MQTT_Pub_Sub();
    flag.flag_stt = 0;
  }
  if (cnt_mqtt==50)
  {
    To_mqtt_browser();
    cnt_mqtt=0;
  }
}