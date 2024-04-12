#ifndef __HEADER_H__
#define __HEADER_H__

#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <my_difine_DA.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

extern uint8_t cnt_false;
extern uint8_t cnt_delay;
extern uint8_t Card_check;

extern uint8_t Cambien;
extern uint8_t Value;

extern uint8_t But_Fan_livingroom;
extern uint8_t But_Fan_livingroom_old;
extern uint8_t Cnt_But_Fan_livingroom;

extern uint8_t But_Fan_kitchen;
extern uint8_t But_Fan_kitchen_old;
extern uint8_t Cnt_But_Fan_kitchen;

extern uint8_t But_Light_livingroom;
extern uint8_t But_Light_livingroom_old;
extern uint8_t Cnt_But_Light_livingroom;

extern uint8_t But_Light_kitchen;
extern uint8_t But_Light_kitchen_old;
extern uint8_t Cnt_But_Light_kitchen;

extern int check_gas;
extern int cnt_check_gas;
extern int cnt_mqtt;
extern byte nuidPICC[4];
extern byte targetUID[4];

struct Flag
{
    uint8_t flag_start;
    uint8_t flag_stt;
    uint8_t flag_check_id;
    uint8_t flag_delay;
    uint8_t flag_Attach_Fan_manual_livingroom;
    uint8_t flag_Attach_Fan_manual_kitchen;
    uint8_t flag_Attach_Light_manual_livingroom;
    uint8_t flag_Attach_Light_manual_kitchen;
    uint8_t flag_Check_Gas;
    uint8_t flag_Temperature;
};
extern struct Flag flag;

// extern int temp;
// extern int gas;
// extern int Led1;
// extern int Led2;
// extern int Led3;
// extern int Fan1;
// extern int Fan2;
// extern int cnt_mqtt;

void Flag_Status();
void Check_ID();
void Check_id_false();
void Open_door();
void On_Light_auto_living();
void Off_Light_auto_living();
void Dark_Light_Livingroom();
void Attach_Fan_manual_livingroom();
void Attach_Light_manual_livingroom();
void Attach_Light_manual_kitchen();
void Check_Gas();
void Temperature_and_Humidity();
void On_Fan_auto_kitchen();
void Off_Fan_auto_living();
void Temperature_and_Humidity();
void set_pin_mode();
void RFID_Config();
void Servo_set();
void LCD_set();
void DHT_set();


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
void ESP_to_browser_and_else ();
#endif