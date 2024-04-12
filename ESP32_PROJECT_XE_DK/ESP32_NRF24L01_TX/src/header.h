#ifndef __HEADER_H__
#define __HEADER_H__

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SimpleKalmanFilter.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#define URX_Pick_Up 33 // Chân URX
#define URY_Pick_Up 32 // Chân URY
#define SW_Pick_Up 25  // Chân SW
#define URX_Control_Motor 27 // Chân URX
#define URY_Control_Motor 26 // Chân URY
#define POTEN_PIN 15

extern int flag_tx;
struct JoystickData
{
    int x_Pick_Up;
    int y_Pick_Up;
    int switchState_Pick_Up;
    int x_Control_Motor;
    int y_Control_Motor;
    //int Poten;
};
extern struct JoystickData data;
void TX_SETUP() ;
void IRAM_ATTR check_status();
void Setup_Timer();
void JOYSTICK_SETUP();
JoystickData VALUE_JOYSTICK();
void POTEN_SETUP();
void TX_TRANS() ;
#endif