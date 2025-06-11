// #include <Arduino.h>
// #include <Wire.h>
// #define IN1 26
// #define IN2 27
// #define IN3 22
// #define IN4 23
// #define ENA 25
// #define ENB 21
// void L298N_SETUP();
// void setup()
// {
//   L298N_SETUP();
// }
// void loop()
// {
//   // RUN_FWD();
//   analogWrite(ENA, 200);
//   digitalWrite(IN1, HIGH); // tiến
//   digitalWrite(IN2, LOW);
//   analogWrite(ENB, 200);
//   digitalWrite(IN3, HIGH); // tiến
//   digitalWrite(IN4, LOW);
// }
// void L298N_SETUP()
// {
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);
//   pinMode(ENA, OUTPUT);
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);
//   pinMode(ENB, OUTPUT);
// }
#include <ESP32Servo.h>

#define SERVO_PIN 19 // Define the pin to which the servo is connected

Servo servo; // Create a servo object

void setup()
{
  Serial.begin(9600); // Initialize serial communication at 115200 baud rate
  servo.attach(SERVO_PIN); // Attach the servo to the specified pin
}

void loop()
{
  // Sweep the servo from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle++)
  {
    servo.write(angle); // Set the servo position
    delay(15);          // Wait for the servo to reach the position7
    Serial.println(angle);
  }

  // Sweep the servo from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle--)
  {
    servo.write(angle); // Set the servo position
    delay(15);          // Wait for the servo to reach the position
    Serial.println(angle); // Print the current angle
  }
}
