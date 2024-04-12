
#include <header.h>

#define URX_Pick_Up 33 // Chân URX
#define URY_Pick_Up 32 // Chân URY
#define SW_Pick_Up 25  // Chân SW
#define URX_Control_Motor 27 // Chân URX
#define URY_Control_Motor 26 // Chân URY
#define POTEN_PIN 15

void setup()
{
  TX_SETUP();
  // Setup_Timer();
  POTEN_SETUP();
  JOYSTICK_SETUP();
}
void loop(){
  //check_status();
  VALUE_JOYSTICK();
  TX_TRANS();

}
