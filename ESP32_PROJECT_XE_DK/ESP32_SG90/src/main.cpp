// #include <ESP32Servo.h>
 
// Servo servo1;  // create servo object to control a servo
// Servo servo2; 
// Servo servo3; 
// Servo servo4; 
// // Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// int servoPin_1 = 32;
// int servoPin_2 = 4;
// int servoPin_3 = 15;
// int servoPin_4 = 17;
// void setup() {
//   //myservo.setPeriodHertz(50); 
//   servo1.attach(servoPin_1);
//   servo2.attach(servoPin_2);
//   servo3.attach(servoPin_3);
//   servo4.attach(servoPin_4);
// }
 
// void loop() {
//   //servo1.write(0); 
//   //servo2.write(0); 
//   //servo3.write(0); 
//   servo4.write(20); 
//   delay(500);
//   servo4.write(0); 
//   delay(500);
//     // Đợi 1 giây
// }
#include <ESP32Servo.h>

Servo myServo;  // Tạo một đối tượng servo

void setup() {
  Serial.begin(115200);
  myServo.attach(12);  // Gắn servo vào chân 12 của ESP32
}

void loop() {
  // int servoAngle = analogRead(12); // Đọc giá trị từ dây cam
  // int mappedAngle = map(servoAngle, 0, 1023, 0, 180); // Ánh xạ giá trị đọc sang góc
  // Serial.print("Goc hien tai: ");
  // Serial.println(mappedAngle);
  // delay(1000); // Đợi 1 giây trước khi đọc lại
  int currentAngle = myServo.read(); // Đọc góc hiện tại của servo
  Serial.print("Goc hien tai: ");
  Serial.print(currentAngle);
  Serial.println(" degrees");
  
  delay(1000);
}
