#include <header.h>

// 5 16 17 18 19 23 // nrf
// 34 35 // cs 100a
// 25 32 33 17
void setup() 
{
  NRF_RX_SETUP();
  L298N_SETUP();
  SERVO_SETUP();
  Setup_Timer();
}
void loop() 
{
  check_status();
  RUN_RX();
  CS100A_RUN();
  RUN_FWD();
  SERVO_RUN();
}

