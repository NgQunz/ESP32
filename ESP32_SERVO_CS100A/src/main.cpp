#include <Arduino.h>
#include <NewPing.h>
#include <Wire.h>

// Định nghĩa chân kết nối với cảm biến CS100A
#define TRIG_PIN_1  16  // Chân Trig của cảm biến
#define ECHO_PIN_1  17 // Chân Echo của cảm biến
#define TRIG_PIN_2  32  // Chân Trig của cảm biến
#define ECHO_PIN_2  33 // Chân Echo của cảm biến
#define TRIG_PIN_3  34  // Chân Trig của cảm biến
#define ECHO_PIN_3  35 // Chân Echo của cảm biến
#define MAX_DISTANCE 200 // Khoảng cách tối đa để đo (cm)

NewPing sonar(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // Tạo đối tượng NewPing

void setup() {
  Serial.begin(9600); // Khởi động Serial để in kết quả ra màn hình
}

void loop() {
  // Đo khoảng cách và lưu kết quả
  int distance = sonar.ping_cm();

  // Kiểm tra nếu có kết quả hợp lệ (không phải 0)
  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Out of range");
  }

  delay(1000); // Delay trước khi đo lần tiếp theo
}

// float leftDistance=0,midDistance=0,rightDistance=0;

// void CS100A_SETUP();
// void Doc_khoang_cach();
// void setup()
// {
//   Serial.begin(9600);
//   CS100A_SETUP();
// }

// void loop()
// {
//   Doc_khoang_cach();
// }
// void Doc_khoang_cach()
// {
//   float leftDistance = sonar[0].ping_cm();
//   float midDistance = sonar[1].ping_cm();
//   float rightDistance = sonar[2].ping_cm();
//   Serial.printf("left: %.2f cm\n", leftDistance);
//   Serial.printf("right: %.2f cm\n", rightDistance);
//   Serial.printf("mid: %.2f cm\n", midDistance);
// }

// void CS100A_SETUP()
// {
//   NewPing sonar[0] = {NewPing(34, 35, MAX_DISTANCE)};// 34 TRig, 35 Echo  //trái
//   NewPing sonar[1] = {NewPing(25, 26, MAX_DISTANCE)};// 25 TRig, 26 Echo  //giữa
//   NewPing sonar[2] = {NewPing(32, 33, MAX_DISTANCE)};// 32 TRig, 33 Echo  //phải
// }
