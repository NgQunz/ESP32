//#define sh 13
//#define ds 12
//#define st 11
//  byte led7_Hex[]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF0, 0x80, 0x90}
//}
//  void setup() {
//  pinMode(
//
//}
//
//void loop() {
//    int i;
//    for(i = 0; i < 8; i++) {
//        if(bitOrder == LSBFIRST)
//        {
//          digitalWrite(dataPin, !!(val & (1 << i)));
//        }
//        else
//        {
//          digitalWrite(dataPin, !!(val & (1 << (7 - i))));
//        }
//        digitalWrite(clockPin, HIGH);
//        digitalWrite(clockPin, LOW);
//    }
//}
// #define sh 13
// #define ds 12
// #define st 11

// byte mahex_a[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// void setup() {
//   pinMode(sh, OUTPUT);
//   pinMode(ds, OUTPUT);
//   pinMode(st, OUTPUT);
//   // pinMode(9, OUTPUT);
// }

// void loop() {
//   digitalWrite(9,HIGH);
//   for (int i = 0; i < 10; i++) {
//     digitalWrite(st, LOW);
//     shiftOut(ds, sh, MSBFIRST, mahex_a[i]);
//     digitalWrite(st, HIGH);
//     delay(1000);
//   }
// }
// #define sh 13
// #define ds 12
// #define st 11
// byte mahex_a[]={0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
// void setup() {
//   pinMode(sh,OUTPUT);
//   pinMode(ds,OUTPUT);
//   pinMode(st,OUTPUT);

// }
// void loop() {
//   for(int i=0;i<8;i++){
//     digitalWrite(st,LOW);
//     shiftOut(ds,sh,MSBFIRST,mahex_a[i]);
//     digitalWrite(st,HIGH);
//     delay(1000);
//   }
// }
#include <Arduino.h>
#define sh 25
#define ds 26
#define st 27
// bool bitRead1(int bit);
byte left_to_right_arr[] = { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };
byte fly_arr[] = { 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0x7E, 0x3C, 0x18 };
byte water_arr[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
                     0x81, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0x83,
                     0x43, 0x23, 0x13, 0x0B, 0x07, 0x87, 0x47, 0x27,
                     0x17, 0x0F, 0x8F, 0x4F, 0x2F, 0x1F, 0x9F, 0x5F,
                     0x3F, 0xBF, 0x7F, 0xFF };
byte snake_arr[] = { 0x81, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0xC1,
                     0x61, 0x31, 0x19, 0x0D, 0x07, 0xE1, 0x71, 0x39,
                     0x1D, 0x0F, 0xF1, 0x79, 0x3D, 0x1F, 0xF9, 0x7D,
                     0x3F, 0xFD, 0x7F, 0xFF };
void left_to_right();
void fly();
void water();
void snake();
void customShiftOut(int dataPin, int clockPin, int bitOrder, byte value);

void setup() {
  pinMode(sh, OUTPUT);
  pinMode(ds, OUTPUT);
  pinMode(st, OUTPUT);
}

void loop() {
  left_to_right();
}

void left_to_right() {
  for (int i = 0; i < 8; i++) {
    shiftOut(ds, sh, LSBFIRST, left_to_right_arr[i]);
  }
}

void water() {
  for (int i = 0; i < 36; i++) {
    customShiftOut(ds, sh, LSBFIRST, water_arr[i]);
  }
}

void fly() {
  for (int i = 0; i < 8; i++) {
    customShiftOut(ds, sh, LSBFIRST, fly_arr[i]);
  }
}

void snake() {
  for (int i = 0; i < 28; i++) {
    customShiftOut(ds, sh, LSBFIRST, snake_arr[i]);
  }
}
// bool bitRead1(int bit) {
//   return ((led_BIT >> bit) & 1); // thêm & 1 để trả về 0 và 1
// }
void customShiftOut(int dataPin, int clockPin, int bitOrder, byte value) {
  digitalWrite(st, LOW);
  for (int i = 0; i < 8; i++) {
    int bitToSend;
    if (bitOrder == MSBFIRST) {  // đọc mã bin từ phải qua trái
      bitToSend = (value & (1 << (7 - i)));
    } else {
      bitToSend = (value & (1 << (i)));  // đọc mã bin từ trái qua phải
    }
    digitalWrite(dataPin, bitToSend);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(st, HIGH);
  delay(200);
}