#ifndef __HEADER_H__
#define __HEADER_H__

#include <my_define.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
extern int temp;
extern int gas;
extern int Led1;
extern int Led2;
extern int Led3;
extern int Fan1;
extern int Fan2;
extern int Fan3;
extern int count;
extern int cnt;
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
void CHE_DO();
void On_LED(int PIN);
void Off_LED(int PIN);
void On_FAN(int PIN);
void Off_FAN(int PIN);
float Temp_Read();
void SET_ALL();
void Hien_thi_LCD();
int Gas_Read();
void LED_AUTO();
int Button_Read(int );
void AUTO_ALL();
void Use_Button_to_Control();
void MQTT_Control();
#endif
