
#include <header.h>

// RF24 radio(16, 5); // CE, CSN
// const byte diachi[6] = "12345";
// #define IN1 26
// #define IN2 27
// #define ENA 4
// #define IN3 21
// #define IN4 22
// #define ENB 13
// 5 16 17 18 19 23 // nrf
// 34 35 // cs 100a
// 25 32 33 17
// struct JoystickData {
//   int x_Pick_Up;
//   int y_Pick_Up;
//   int switchState_Pick_Up;
//   int x_Control_Motor;
//   int y_Control_Motor;
//   int Poten;
// };
// void RUN_RX();
// void NRF_RX_SETUP();
void setup() 
{
  NRF_RX_SETUP();
  L298N_SETUP();
}
void loop() 
{
  check_status();
  RUN_RX();
  RUN_FWD();
}
// int flag_run_fwd=1;
