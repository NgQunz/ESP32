#include <Arduino.h>

// Khai báo các chân GPIO cho LED đơn 7 đoạn
const int segA = 21;
const int segB = 19;
const int segC = 14;
const int segD = 27;
const int segE = 26;
const int segF = 25;
const int segG = 33;

// Khai báo mảng lưu trữ giá trị hiển thị của các chữ số từ 0 đến 9
const byte digits[10][7] = {
  {0, 0, 0, 0, 0, 0, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0}, // 2
  {0, 0, 0, 0, 1, 1, 0}, // 3
  {1, 0, 0, 1, 1, 0, 0}, // 4
  {0, 1, 0, 0, 1, 0, 0}, // 5
  {0, 1, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0}, // 8
  {0, 0, 0, 0, 1, 0, 0}  // 9
};

// Hàm khởi tạo các chân GPIO và đặt chúng là OUTPUT
void setup() {
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
}

// Hàm hiển thị một chữ số lên LED đơn 7 đoạn
void displayDigit(byte digit) {
  digitalWrite(segA, digits[digit][0]);
  digitalWrite(segB, digits[digit][1]);
  digitalWrite(segC, digits[digit][2]);
  digitalWrite(segD, digits[digit][3]);
  digitalWrite(segE, digits[digit][4]);
  digitalWrite(segF, digits[digit][5]);
  digitalWrite(segG, digits[digit][6]);
}

void loop() {
  // Hiển thị các chữ số từ 0 đến 9 lần lượt
  for (int i = 0; i < 10; i++) {
    displayDigit(i);
    delay(1000); // Độ trễ 1 giây
  }
}
#include <Arduino.h>

const int digitPins[] = {2, 3, 4, 5};   // Chân điều khiển các chữ số
const int segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; // Chân điều khiển các segment

const byte digitMap[] = {
  B00111111,  // 0
  B00000110,  // 1
  B01011011,  // 2
  B01001111,  // 3
  B01100110,  // 4
  B01101101,  // 5
  B01111101,  // 6
  B00000111,  // 7
  B01111111,  // 8
  B01101111   // 9
};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
}

void displayNumber(int number) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
    byte segments = digitMap[number % 10];
    for (int j = 0; j < 8; j++) {
      digitalWrite(segmentPins[j], bitRead(segments, j));
    }
    delay(5);
    for (int j = 0; j < 8; j++) {
      digitalWrite(segmentPins[j], LOW);
    }
    digitalWrite(digitPins[i], HIGH);
    number /= 10;
  }
}

void loop() {
  for (int i = 0; i < 100; i++) {
    displayNumber(i);
    delay(1000);
  }
}
