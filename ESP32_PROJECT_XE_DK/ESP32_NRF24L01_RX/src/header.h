#ifndef __HEADER_H__
#define __HEADER_H__

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#define IN1 26
#define IN2 27
#define ENA 4
#define IN3 21
#define IN4 22
#define ENB 13
void RUN_RX();
void NRF_RX_SETUP();
void L298N_SETUP() ;
void RUN_FWD() ;
void Motor_off();
void IRAM_ATTR check_status();
void Setup_Timer();
struct JoystickData
{
    int x_Pick_Up;
    int y_Pick_Up;
    int switchState_Pick_Up;
    int x_Control_Motor;
    int y_Control_Motor;
    int Poten;
};
extern struct JoystickData receivedData;
extern int flag_tx;
extern int flag_motor;
#endif