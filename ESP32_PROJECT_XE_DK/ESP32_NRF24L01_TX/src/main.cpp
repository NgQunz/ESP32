
#include <header.h>

#define URX_Pick_Up 33 // Chân URX
#define URY_Pick_Up 32 // Chân URY
#define SW_Pick_Up 25  // Chân SW
#define URX_Control_Motor 27 // Chân URX
#define URY_Control_Motor 26 // Chân URY
#define POTEN_PIN 15
int LED[6]={13,12,11,10,9,8};
void setup()
{
  check_status();
  Setup_Timer();
  POTEN_SETUP();
  TX_SETUP();
  JOYSTICK_SETUP();
}
void loop(){
  VALUE_JOYSTICK();
  TX_TRANS();

}
