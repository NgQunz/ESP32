#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <SPI.h>
#include <Arduino.h>


hw_timer_t *Timer0_Cfg=NULL;
uint32_t Freq=0;
uint8_t flag_check=0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int seconds = 0, minutes = 0, hours = 0;
int cn= 0, cnt_sec = 0;
int btn = 0;
int cnt_alarm = 0;
int alarmHour = 23; 
int alarmMinute = 59; 
uint8_t stt_btn1 = 0, cnt_BT = 0;
int setupValues[6] = {0};
time_t t;
int maxValues[] = {59, 59, 23, 31, 12, 2099};
int minValues[] = {0, 0, 0, 1, 1, 2000};
void IRAM_ATTR Set_Flag()
{
  flag_check=1;
}
int setupMode = false;
void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(SETUP, INPUT);
  pinMode(INC, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(HORN, OUTPUT);
  setTime(10, 0, 0, 28, 7, 2023);  // 10:00:00 28/7/2023
}
void printDigits(int digits) {
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}
void loop()
{
  time_t t = now();
  int setupButtonState = digitalRead(SETUP);
  int incButtonState = digitalRead(INC);
  int downButtonState = digitalRead(DOWN);
  int lastSetupButtonState = HIGH;
  if (setupButtonState == LOW && lastSetupButtonState == HIGH)
  {
    lastSetupButtonState = setupButtonState;
    setupMode = true;
    setupValues[0] = second(t);
    setupValues[1] = minute(t);
    setupValues[2] = hour(t);
    setupValues[3] = day(t);
    setupValues[4] = month(t);
    setupValues[5] = year(t);
  }
  if (setupMode)
  {
    if (setupIndex==5)
    {
      lcd.setCursor(12, 1);
    }
    else
    {
      lcd.setCursor(14, 1); 
    }
    printDigits(setupValues[setupIndex]);
    if (setupButtonState == LOW && !digitalRead(SETUP))
    {
      delay(250);
      setupIndex++;
    }
    if (incButtonState == LOW && !digitalRead(INC)) // Nhấn rồi thả
    {
      delay(250);
      setupValues[setupIndex]++;
      if (setupValues[setupIndex] > maxValues[setupIndex])
        setupValues[setupIndex] = minValues[setupIndex];
    }
    if (downButtonState == LOW && !digitalRead(DOWN)) // Nhấn rồi thả
    {
      delay(250);
      setupValues[setupIndex]--;
      if (setupValues[setupIndex] < minValues[setupIndex])
        setupValues[setupIndex] = maxValues[setupIndex];
    }
    if (setupIndex == 6)
    {
      setupMode = false;
      setupIndex = -1;
    }
    setTime(setupValues[2], setupValues[1], setupValues[0], setupValues[3], setupValues[4], setupValues[5]);
  }
  else 
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    printDigits(day(t));
    lcd.print("/");
    printDigits(month(t));
    lcd.print("/");
    printDigits(year(t));
    lcd.setCursor(0, 1);
    printDigits(hour(t));
    lcd.print(":");
    printDigits(minute(t));
    lcd.print(":");
    printDigits(second(t));
  }
  delay(100); 
}
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Arduino.h>

hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
uint8_t flag_stt = 0;
int seconds = 0, minutes = 0, hours = 0;
int cn= 0, cnt_sec = 0;
int btn = 0;
int cnt_alarm = 0;
int alarmHour = 23; 
int alarmMinute = 59; 
uint8_t stt_btn1 = 0, cnt_BT = 0;

void IRAM_ATTR Dong_ho()
{
  flag_stt = 1;
}

void Che_do1();
void Che_do2();
void chinh_Gio();
void chinh_Phut();
void chinh_TG_Gio();
void chinh_TG_Phut();
void Baothuc();

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int buttonSetupPin = 14; 
const int buttonIncreasePin = 27; 
const int buttonDecreasePin = 26; 
const int buzzerPin = 19; 

bool isSetupMode = false; 

void setup() {
  Wire.begin();
  lcd.init();
  Serial.begin(9600);

  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &Dong_ho, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);

  pinMode(buttonSetupPin, INPUT_PULLUP);
  pinMode(buttonIncreasePin, INPUT_PULLUP);
  pinMode(buttonDecreasePin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  alarmHour = 0;
  alarmMinute = 0;

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello!");
  delay(2000);

}

void loop() {
  if(flag_stt) {
    btn = 1;
    cnt_sec++;
    cnt_alarm++;
    flag_stt = 0;
    stt_btn1 = 0;
  }
  if(cnt_alarm == 5){
    stt_btn1 = 1;
    cnt_alarm = 0;
  }
  if(cnt_sec == 10){
    seconds++;
    cnt_sec = 0;
  }
  if(seconds == 60) {
    seconds = 0;
    minutes++;
    if (minutes == 60) {
      minutes = 0;
      hours++;
      if (hours == 24) {
        hours = 0;
        
      }
    }
  }
  if()
  {

  }
  if(btn){
    btn=0;
    uint8_t stt_btn = digitalRead(buttonSetupPin);
    delay(100);
    if(!stt_btn){
      stt_btn = 1;
      cn++;
    }
  }


  if(cn == 0){
    Che_do1();
  }
  else if(cn == 1){
    Che_do1();
    chinh_TG_Gio();
  }
  else if (cn == 2){
    Che_do1();
    chinh_TG_Phut();
  }
  else if (cn == 3){
    Che_do2();
    chinh_Gio();
  }
  else if(cn == 4){
    Che_do2();
    chinh_Phut();
  }
  else if(cn == 5){
    cn = 0;
    Che_do1();
  }

  Baothuc();

}

  
void Che_do1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(hours < 10 ? "0" : "");
  lcd.print(hours);
  lcd.print(":");
  lcd.print(minutes < 10 ? "0" : "");
  lcd.print(minutes);
  lcd.print(":");
  lcd.print(seconds < 10 ? "0" : "");
  lcd.print(seconds);
  lcd.setCursor(0, 1);
  lcd.print(cn);
}

void Che_do2() {
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("Bao thuc:");
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(alarmHour < 10 ? "0" : "");
  lcd.print(alarmHour);
  lcd.print(" M:");
  lcd.print(alarmMinute < 10 ? "0" : "");
  lcd.print(alarmMinute);
  lcd.setCursor(15, 1);
  lcd.print(cn);
} 

void chinh_Gio() {
  cnt_BT = 0;
  if(btn){
    if(digitalRead(buttonIncreasePin) == LOW){
      if(alarmHour < 23){
        alarmHour++;
      }
    }
    if(digitalRead(buttonDecreasePin) == LOW){
      if(alarmHour > 0){
        alarmHour--;
      }
    }
  }
}

void chinh_Phut() {
  if(btn){
    if(digitalRead(buttonIncreasePin) == LOW){
      if(alarmMinute < 59){
        alarmMinute++;
      }
    }
    if(digitalRead(buttonDecreasePin) == LOW){
      if(alarmMinute > 0){
        alarmMinute--;
      }
    }
  }
}

void chinh_TG_Gio() {
  if(btn){
    if(digitalRead(buttonIncreasePin) == LOW){
      if(hours < 23){
        hours++;
      }
    }
    if(digitalRead(buttonDecreasePin) == LOW){
      if(hours > 0){
        hours--;
      }
    }
  }
}

void chinh_TG_Phut() {
  if(btn){
    if(digitalRead(buttonIncreasePin) == LOW){
      if(minutes < 59){
        minutes++;
      }
    }
    if(digitalRead(buttonDecreasePin) == LOW){
      if(minutes > 0){
        minutes--;
      }
    }
  }
}

void Baothuc() {
  if(!cn){
    if(hours == alarmHour && minutes == alarmMinute){
      if(stt_btn1){
        digitalWrite(buzzerPin, HIGH);
        cnt_BT++;
      }
      if(cnt_BT >20){
        digitalWrite(buzzerPin, LOW);
      }
      Serial.println(String(cnt_BT));
    }
  }
  
}

