#ifndef __MY_DEFINE_H__
#define __MY_DEFINE_H__

#define DHTPIN  15
#define DHTTYPE DHT11

#define SS_PIN 5
#define RST_PIN 27

#define relayPin_Fan_livingroom 16
#define relayPin_Light_livingroom 33

#define relayPin_Fan_kitchen 32

#define relayPin_Light_kitchen 17

#define but_fan_living 26
#define but_light_living 13
#define but_fan_kitchen 14
#define but_light_kitchen 21

#define Gas 4


#define SSID1 "ESP32"
#define PASSWORD "12345678"
#define MQTT_BROKER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define TOPIC_PUB "Esp/Mqt"
#define TOPIC_SUB "Mqt/Esp"
#define MQTT_USERNAME "Quan"
#define MQTT_PASSWORD "Nquan"


#endif