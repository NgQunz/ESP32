#include "header.h"

#define SS_PIN 5
#define RST_PIN 0

Servo servo_door;
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

const int ip[4] = { 212, 94, 20, 184}; //mã
int flag_checkRFID = 1;
int count_wrong = 0;
int count_bell = 0;
int count_light = 0;
int flag_open_door = 0;

int door = 12;
int door_light = 27;
int door_bell = 14;
int pos = 0;
int i = 0;

void Declare_Door()
{
    pinMode( door_light, OUTPUT);
    pinMode( door_bell, OUTPUT);
    servo_door.attach(door);
    servo_door.write(108);
}

void Start_RFID ()
{
    SPI.begin();
    rfid.PCD_Init();
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
}

void Check_RFID() 
{
  for (byte i = 0; i < 6; i++) 
  {
    key.keyByte[i] = 0xFF;
  }
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (  !rfid.PICC_ReadCardSerial())
    return;
  for (byte i = 0; i < 4; i++) 
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  if ( nuidPICC[0] == ip[0] && nuidPICC[1] == ip[1] && nuidPICC[2] == ip[2] && nuidPICC[3] == ip[3] )
  {
    flag_open_door = 1;
  }
  else 
  {
    count_wrong++;
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void Open_Door()
{
  pos -= 4;
  if ( pos > -108 )
  {
    i = 108 - abs(pos);
  }
  else if ( pos <= -108 && pos > -308 )
  {
    i = 0;
  }
  else if ( pos <= -308 && pos > -416)
  {
    i = abs(pos) - 308;
  }
  else if ( pos == -416 )
  {
    i = 108;
    pos = 0;
    flag_open_door = 0;
  }
  servo_door.write(i);
}

void Wrong_RFID()
{
  if ( count_wrong >= 3)
  {
    digitalWrite(door_bell, HIGH);
    count_bell++;
    if ( count_bell >= 40 )
    {
      count_bell = 0;
      count_wrong = 0;
      digitalWrite(door_bell, LOW);
    }
  }
}

void Door_Light()
{
  if ( Night() == 1 && digitalRead( PIN_PROX ) == 0)
  {
    digitalWrite( door_light, HIGH);
  }
  else 
  {
    count_light++;
    if ( count_light >= 50)
    {
      count_light = 0;
      digitalWrite( door_light, LOW);
    }  
  }
}