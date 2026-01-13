#include "Motor_control_basic.h"
#include "Arm_Servo.h"

void setup()
{
  MotorInit();
  ArmInit();
}

void loop()
{
  Nhiemvu3();
  delay(2000);
}
