#ifndef _HEADER_H_
#define _HEADER_H_

#include <Arduino.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h> 
#include <MFRC522.h>
#include <SPI.h>

#define PIN_GAS 36
#define PIN_LIGHT 25
#define PIN_NTC 39
#define PIN_PROX 2
#define SS_PIN 5
#define RST_PIN 0

extern int flag_pause;
extern int count_timer;

extern int sttnew27;
extern int sttnew26;
extern int sttnew32;
extern int sttnew33;
extern int sttnew34;

extern int flag_checkRFID;
extern int count_wrong;
extern int flag_open_door;

extern double t; 
extern int gas;

extern int flag_auto;

extern int flag_living_fan;
extern int flag_living_light;
extern int flag_kitchen_fan;
extern int flag_kitchen_light;
extern int living_fan;
extern int living_light;
extern int kitchen_fan;
extern int kitchen_light;

extern String Home_Data;
extern String receive;

void Declare_Button();
int Press_Button35();
int Press_Button26();
int Press_Button32();
int Press_Button33();
int Press_Button34();

void Declare_Door();
void Start_RFID();
void Check_RFID();
void Open_Door();
void Wrong_RFID();
void Door_Light();  

void IRAM_ATTR check_status();
void Setup_Timer();

void Temp ();
void Declare_Sensor();
void Gas();
int Night();

void Declare_Device();
void Living_Auto ();
void Kitchen_Auto();
void Manual_Button();
void Solve_Device();

void Start_Lcd();
void Print_Temp();
void Print_Gas();
void Print_Funtion();

void Setup_Wifi();
void Data_to_Json(int t, int flag_living_fan, int flag_living_light, int flag_kitchen_fan, int flag_kitchen_light);
void Detext(String Str);
void MQTT_Pub_Sub();
void MQTT_Sub();
void callback(char *topic, byte *payload, unsigned int length);
void MQTT_Connect();
void Client_Loop();



#endif



