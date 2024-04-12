// #include <Arduino.h>
// #define PIN_VR 34
// #define PIN_CB 13
// int a[]={4,16,17,18,19,27,32,22};
// // float map_a (int value, int in_min, int in_max, float out_min, float out_max)
// // {
// //   return (float) (value-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
// // }
// float value_ADC = 0;
// float Sum = 0;

// void setup(){
//   Serial.begin(9600);
//   pinMode(PIN_VR,INPUT);
//    pinMode(PIN_CB,INPUT);
//   for (int i=0;i<8;i++)
//   {
//     pinMode(a[i], OUTPUT);
//     digitalWrite(a[i],1);
//   }
// }
// void Cam_bien_khi_ga()
// {
//   for(int i = 0; i < 10; i++)
//   {
//     value_ADC =analogRead(PIN_CB);
//     Sum += value_ADC;
//     delay(50);
//   }
//   Sum/=10;
//   Serial.println("Value ADC: " + String(map(Sum,0,4095,0,100)));
//   Sum=0;
// }
// void Cam_bien_as()
// {
//   int value=digitalRead(PIN_CB);
//   Serial.println("Value ADC: " + String(value));
//   delay(5);
//   if (value==1)
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],0);
//     }
//   }
//   else
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],1);
//     }
//   }
// }
// void Cam_bien_do_am()
// {
//   int value=digitalRead(PIN_CB);
//   Serial.println("Value ADC: " + String(value));
//   delay(5);
//   if (value==1)
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],0);
//     }
//   }
//   else
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],1);
//     }
//   }
// }
// void Cam_bien_mua()
// {
//   int value=digitalRead(PIN_CB);
//   Serial.println("Value ADC: " + String(value));
//   delay(5);
//   if (value==1)
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],0);
//     }
//   }
//   else
//   {
//     for (int i=0;i<8;i++)
//     {
//       pinMode(a[i], OUTPUT);
//       digitalWrite(a[i],1);
//     }
//   }
// }
// void loop(){
  
//   // Cam_bien_as();
//   // Cam_bien_khi_ga();
//   Cam_bien_do_am();

// }
#include <Arduino.h>
void setup() {
  pinMode(4, OUTPUT);
  digitalWrite(4,0);
}

void loop() {
  digitalWrite(4,1);
  delay(1000);
  digitalWrite(4,0);
  delay(1000);
}