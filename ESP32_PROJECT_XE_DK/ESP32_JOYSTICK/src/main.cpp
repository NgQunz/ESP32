// #include <Arduino.h>
// #include <SimpleKalmanFilter.h>
// SimpleKalmanFilter bo_loc(2, 2, 0.001);
// // Khai báo các chân kết nối joystick

// const int URX_Pick_Up = 33; // Chân URX
// const int URY_Pick_Up = 32; // Chân URY
// const int SW_Pick_Up = 25;  // Chân SW
// const int URX_Control_Motor = 27; // Chân URX
// const int URY_Control_Motor = 26; // Chân URY
// //1 x=0 tại x khoảng -5 đến -20 
// //1 y từ -10 từ 10
// struct JoystickData {
//   int xMapped_Pick_Up;
//   int yMapped_Pick_Up;
//   int switchState_Pick_Up;
//   int xMapped_Control_Motor;
//   int yMapped_Control_Motor;
// };
// Serial.begin(115200);
// void JOYSTICK_SETUP() {
//   // Khởi tạo Serial Monitor
//   Serial.begin(9600);

//   pinMode(URX_Pick_Up, INPUT);
//   pinMode(URY_Pick_Up, INPUT);
//   pinMode(SW_Pick_Up, INPUT_PULLUP); // Chân SW với chế độ INPUT_PULLUP
//   pinMode(URX_Control_Motor, INPUT);
//   pinMode(URY_Control_Motor, INPUT);

// }
// JoystickData VALUE_JOYSTICK() {
//   JoystickData data;
//   int xValue_Pick_Up = analogRead(URX_Pick_Up);
//   int yValue_Pick_Up = analogRead(URY_Pick_Up);
//   data.xMapped_Pick_Up = map(xValue_Pick_Up, 0, 4095, -255, 255);
//   data.yMapped_Pick_Up = map(yValue_Pick_Up, 0, 4095, -255, 255);
//   data.switchState_Pick_Up = digitalRead(SW_Pick_Up);
//   int xValue_Control_Motor = analogRead(URX_Control_Motor);
//   int yValue_Control_Motor = analogRead(URY_Control_Motor);
//   data.xMapped_Control_Motor = -map(xValue_Control_Motor, 0, 4095, -255, 255);
//   data.yMapped_Control_Motor = map(yValue_Control_Motor, 0, 4095, -255, 255);
//   // Serial.printf("%d %d\n",data.xMapped_Pick_Up ,data.yMapped_Pick_Up );
//   Serial.printf("%d %d\n",data.xMapped_Control_Motor,data.yMapped_Control_Motor );
//   delay(500);
//   return data;
// }
// void setup()
// {
//   JOYSTICK_SETUP();
// }
// void loop(){
//   VALUE_JOYSTICK();
// }
#include "RTClib.h"

const int ledPins = 13;
int count=0;
const int BTN_PIN = 12;
hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
int flag_stt=0;
int sttnew = 1;
void setup() {
  Serial.begin(9600);
    pinMode(ledPins, OUTPUT); // Set all LED pins as OUTPUT
    digitalWrite(ledPins, count);
    pinMode(BTN_PIN, INPUT);
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
}

void IRAM_ATTR check_status()
{
  flag_stt=1;
}
void loop() {
  check_status();
  if (flag_stt==1)
  {
    int sttold = sttnew;
    sttnew = digitalRead(BTN_PIN);
    if ( sttold == 1 && sttnew == 0)
    {
      count++;  
      count%=2;
      digitalWrite(ledPins, count);
    }
    flag_stt=0;
  }
   
}