#include <Servo.h>  
#include <Arduino.h>

Servo myservo;          
 
#define PIN_BT 34  
#define PIN_SERVO 13
void setup ()
{
    myservo.attach(PIN_SERVO); 
    pinMode(PIN_BT,INPUT);
    Serial.begin(9600);
}
 
void loop ()
{
    int value=0,sum=0;
    for (int i=0;i<50;i++)
    {
      value = analogRead(PIN_BT);
      sum+=value;
    }
    sum/=50;
    int servoPos = map(sum, 0, 4095, 0, 180);

    for (int i=0;i<=servoPos;i++)
    {
      myservo.write(i);
      delay(10);
    }
    delay(100);
    for (int i=servoPos;i>=0;i--)
    {
      myservo.write(i);
      delay(10);
    }
    delay(100);
    Serial.println(String(servoPos));
}
