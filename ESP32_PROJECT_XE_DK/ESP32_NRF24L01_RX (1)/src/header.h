#ifndef __HEADER_H__
#define __HEADER_H__

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>
#include <NewPing.h>
#include <ESP32Servo.h>

#define IN1 26
#define IN2 27
#define ENA 4
#define IN3 21
#define IN4 22
#define ENB 13
#define SERVO1_PIN 25
#define SERVO2_PIN 17
#define SERVO3_PIN 32
#define SERVO4_PIN 33
#define MAX_DISTANCE 400 
#define Trig 34
#define Echo 35


void RUN_RX();
void NRF_RX_SETUP();
void L298N_SETUP() ;
void RUN_FWD() ;
void Motor_off();
void IRAM_ATTR check_status();
void Setup_Timer();
void SERVO_SETUP();
void SERVO_RUN();
void CS100A_RUN();
void CS100A_SETUP();
struct JoystickData
{
    int x_Pick_Up;
    int y_Pick_Up;
    int switchState_Pick_Up;
    int x_Control_Motor;
    int y_Control_Motor;
    int Poten;
};

// extern Servo servo1;
// extern Servo servo2;
// extern Servo servo3;
// extern Servo servo4;
// extern NewPing sonar ;
extern struct JoystickData receivedData;
extern int flag_tx;
extern int flag_motor;
extern int flag_joy;
extern int flag_barrier;
extern int flag_servo;
extern int flag_cs100a;
extern int x_Corner, y_Corner, x_Value, y_Value, z_Corner;
extern int Button_State ;
extern int Count ;
extern int Button_Default;
extern float distance;
#endif