#include <Arduino.h>
#include <user_define.h>
#include <ESP32Servo.h>

Servo myservo; 
int pos = 90;

void setup()
{

  Serial.begin(9600);
  myservo.attach(PIN_SERVO);
  pinMode(PIN_RL, OUTPUT);
  digitalWrite(PIN_RL, LOW);
  delay(1000);
  digitalWrite(PIN_RL, HIGH);
  delay(1000);
  pinMode(IN_L1, INPUT);
  pinMode(IN_L2, INPUT);
  pinMode(IN_L1, INPUT);
  myservo.write(90);
}

char str[11], i = 0;
char inByte = 0;
String data;
void loop()
{

  if (Serial.available())
  {
    while (Serial.available() > 0)
    {
      inByte = Serial.read();
      if (inByte != '\n')
      {
        str[i++] = inByte;
        data += inByte;
      }
      else
      {
        str[i++] = '\0';
        // Serial.println("Str: " + String(str));
        // Serial.println("String: " + data);
        // Serial.println("Length: " + String(strlen(str)));
        if (strstr(str, "Touch:") != NULL && strlen(str) >= 7)
        {
          String data_touch = data.substring(6);
          // printf("%d\n", data_touch.toInt());
          for(int j = 0; j < data_touch.toInt(); j++){
            // Serial.println(j);
            myservo.write(POS_Press);
            delay(800);
            myservo.write(90);   
            delay(2000);  
          }
        }
        if (strstr(str, "Degree:") != NULL)
        {
          String data_degree = data.substring(7);
          // printf("%d\n", data_degree.toInt());
          myservo.write(data_degree.toInt());
        }
        if (strstr(str, "GetStatus"))
        {
          Serial.println(String(digitalRead(IN_L1)) + String(digitalRead(IN_L2)) + String(digitalRead(IN_L3)));
        }
        if (strstr(str, "Press")!=NULL)
        {
          // Serial.println("Servo press");
          myservo.write(POS_Press);
        }
        if (strstr(str, "Release")!=NULL)
        {
          // Serial.println("Servo Release");
          myservo.write(90);
        }
        if (strstr(str, "Touch")!=NULL && strlen(str) <= 6)
        {
          // Serial.println("ON TOUCH");
          myservo.write(POS_Press);
          delay(1000);
          myservo.write(90);   
        }
        if (strstr(str, "Relay:ON")!=NULL)
        {
          // Serial.println("Relay: ON");
          digitalWrite(PIN_RL, LOW);
        }
        if (strstr(str, "Relay:OFF")!=NULL)
        {
          // Serial.println("Relay: OFF");
          digitalWrite(PIN_RL, HIGH);
        }      
        data.remove(0, data.length());
        memset(str, '\0', strlen(str));
        i = 0;
      }
    }
  }
}
