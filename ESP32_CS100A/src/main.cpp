#include <Arduino.h>
#include<NewPing.h>

#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping.
NewPing sonar[1]={NewPing(34, 35, MAX_DISTANCE )};

void setup() {
  Serial.begin(115200); 
}


void loop() {
  float  distance = sonar[0].ping_cm();
//   if(distance >=0 && distance <= 10) //flag.flag_cs100a=1;
//   //else flag.flag_cs100a=0;
// {
  Serial.println(distance);
// }
}
// #define g 10

// const int ledPins[] = {8,9,10, 11,12, 13};
// const int numLeds = 6;

// int ballHighest =5;
// int t=0;
// void setup() {
//   // Khởi tạo chân LED là OUTPUT
//   for (int i = 0; i < numLeds; i++) {
//     pinMode(ledPins[i], OUTPUT);
//     digitalWrite(ledPins[i], 1);
//   }
// }

// void loop() {
//   for (int i=ballHighest;i>=0;i--)// đi xuống
//   {
//     digitalWrite(ledPins[i], 0); 
//     if (i!=0)
//     {
//       t=sqrt(2*(ballHighest-i)/g)-sqrt(2*(ballHighest-i-1)/g);
//       t*=1000;
//       delay(t);
//       Serial.print(t);
//     }
//     digitalWrite(ledPins[i], 1);
//   }
//   ballHighest--;
  
//   // for (int i=0;i<=ballHighest;i++) // nảy lên
//   // {
//   //   digitalWrite(ledPins[i], 0);
//   //   if (i!=ballHighest)
//   //   {
//   //     delay(300);
//   //   }
//   //   digitalWrite(ledPins[i], 1);
//   // }
//   // if (ballHighest==0)
//   // {
//   //   digitalWrite(ledPins[0], 0);
//   //   delay(500);
//   //   digitalWrite(ledPins[0], 1);
//   //   ballHighest=5;
//   // }
// }
