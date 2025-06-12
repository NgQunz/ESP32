#include <SoftwareSerial.h>
#include <Arduino.h>
#define RELAY1 5
#define RELAY2 6
#define RELAY3 7

SoftwareSerial hc12(10, 11); // RX, TX

void setup()
{
  Serial.begin(9600);
  hc12.begin(9600);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);

  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
}

void loop()
{
  if (hc12.available())
  {
    String cmd = hc12.readStringUntil('\n');
    cmd.trim();

    Serial.println("Lệnh nhận: " + cmd);

    if (cmd == "bat_den_1")
    {
      digitalWrite(RELAY1, HIGH);
    }
    else if (cmd == "tat_den_1")
    {
      digitalWrite(RELAY1, LOW);
    }
    else if (cmd == "bat_den_2")
    {
      digitalWrite(RELAY2, HIGH);
    }
    else if (cmd == "tat_den_2")
    {
      digitalWrite(RELAY2, LOW);
    }
    else if (cmd == "bat_den_3")
    {
      digitalWrite(RELAY3, HIGH);
    }
    else if (cmd == "tat_den_3")
    {
      digitalWrite(RELAY3, LOW);
    }
  }
}
