#include <ESP32Servo.h>

Servo myServo;
#define Servo_pin 33
void setup()
{
  Serial.begin(115200);
  myServo.attach(Servo_pin);
}

void loop()
{
  int currentAngle = myServo.read();
  for (int i = 0; i < 180; i++)
  {
    myServo.write(i);
    delayMicroseconds(50);
  }
  for (int i = 180; i >= 0; i--)
  {
    myServo.write(i);
    delayMicroseconds(50);
  }
  Serial.print(currentAngle);
  delay(30000);
}
