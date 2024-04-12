#include<header.h>

Servo servo1; // dùng biến trở để quay
Servo servo2; 
Servo servo3;
Servo servo4;

int x_Corner = 0, y_Corner = 0, x_Value = 0, y_Value = 0, z_Corner = 0;
int Button_State = 0;
int Count = 0;
int Button_Default = 1;
 
void SERVO_SETUP()
{
  servo1.attach(SERVO1_PIN); 
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(20);
}

void SERVO_RUN()
{
  if (flag_servo == 1)
  {
    z_Corner= receivedData.Poten;
    servo1.write(z_Corner);
    y_Value= receivedData.y_Pick_Up; 
    if ( y_Value <4095 && y_Corner<140)
    { // đọc giá trị analog của trục Y , servo phải 
      y_Corner= y_Corner + 1;
      servo2.write(y_Corner);
    }
    if ( y_Value >0 && y_Corner >50)
    {
      y_Corner = y_Corner -1;
      servo2.write(y_Corner);
    }
    x_Value = receivedData.x_Pick_Up; 
    if ( x_Value <4095 && x_Corner<120)
    { // đọc giá trị analog của trục x, trái 
      x_Corner = x_Corner +1;
      servo3.write(x_Corner);
    }
    if ( x_Value > 0 && x_Corner>65)
    {
      x_Corner = x_Corner -1;
      servo3.write(x_Corner);
    }
    Button_State=receivedData.switchState_Pick_Up;
    if(Button_State != Button_Default)
    {
      if(Button_State == 0)
      {
        Count++;
      }
      Button_Default = Button_State;
    }
    Serial.println(Count);
    if(Count%2 == 0)   
    {
      servo4.write(20); //kep
    }
    else servo4.write(0); //mo
    flag_servo=0;
  }
}