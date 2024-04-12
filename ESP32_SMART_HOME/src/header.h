#ifndef __HEADER_H__
#define __HEADER_H__

#include <my_define.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

extern int temp;
extern int gas;
extern int Led1;
extern int Led2;
extern int Led3;
extern int Fan1;
extern int Fan2;
extern int Fan3;
extern int count;

void Stay_Connect_MQTT();
void Wifi_Connect();
void MQTT_Connect();
void MQTT_Pub_Sub();
void To_mqtt_browser();
void callback(char *topic, byte *payload, unsigned int length);
void Data_to_Json(int , int , int , int , int , int , int );
void Detext(String Str);
void Xu_ly_den(int );
void Xu_ly_quat(int );

#endif
