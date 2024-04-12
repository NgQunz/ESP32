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
extern double temp;
extern int gas;
extern int Led1;
extern int Led2;
extern int Led3;
extern int Fan1;
extern int Fan2;
extern int Fan3;
extern int count;
extern int cnt;
extern int Button_Regime_Old;
extern int Button_Regime_New;
extern int Button_Led1_Old;
extern int Button_Led1_New;
extern int Button_Led2_Old;
extern int Button_Led2_New;
extern int Button_Led3_Old;
extern int Button_Led3_New;
extern int Button_Fan1_Old;
extern int Button_Fan1_New;
extern int Button_Fan2_Old;
extern int Button_Fan2_New;
struct Flag
{
    uint8_t flag_start;
    uint8_t flag_mqtt;
    uint8_t flag_check_id;
    uint8_t flag_delay;
    uint8_t flag_Gas;
    uint8_t flag_Temp;
    uint8_t flag_che_do;
    uint8_t flag_mqtt_control;
    uint8_t flag_auto_all;
    uint8_t flag_open_door;
    uint8_t flag_but_ctrl;
    uint8_t flag_but_read;
    uint8_t flag_lcd;
    uint8_t flag_led_e3f;
    uint8_t flag_cb_as;
};
extern struct Flag flag;
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
void Led_1(int);
void Led_2(int);
void Led_3(int);
void Fan_1(int);
void Fan_2(int);
float Temp_Read();
void SET_ALL();
void Hien_thi_LCD();
int Gas_Read();
void LED_E3F();
// int Read_Button(int );
void AUTO_ALL();
void Use_Button_to_Control();
void MQTT_Control();
void Open_The_Door();
void Flag_Status();
void Check_id_false();
void Check_ID();
void MQTT_to_browser();
void LED_CB_AS();
int Button_Regime();
int Button_Led1();
int Button_Led2();
int Button_Led3();
int Button_Fan1();
int Button_Fan2();
#endif
