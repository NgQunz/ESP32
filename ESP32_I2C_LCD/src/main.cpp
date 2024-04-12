#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Arduino.h>

hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
uint8_t flag_stt = 0;
int seconds = 0, minutes = 0, hours = 0, days = 1, months = 1, years = 2023;
int cn= 0, cnt_sec = 0;
int btn = 0;
int cnt_alarm = 0;
int alarmHour = 23; 
int alarmMinute = 59; 
uint8_t stt_btn1 = 0, cnt_BT = 0;

// Khai báo đối tượng LiquidCrystal_I2C để điều khiển màn hình LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define SETUP 16
#define INC 17
#define DOWN 18
#define HORN 19
bool isSetupMode = false; 

void IRAM_ATTR Dong_ho()
{
  flag_stt = 1;
}

void setup() {
  Wire.begin();
  lcd.init();
  Serial.begin(9600);

  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &Dong_ho, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);

  pinMode(SETUP, INPUT_PULLUP);
  pinMode(INC, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(HORN, OUTPUT);

  alarmHour = 0;
  alarmMinute = 0;

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello!");
  delay(2000);
}
void printDigits(int digits) {
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}
void Hien_dong_ho() {
  lcd.clear();
  lcd.setCursor(0, 0);
   lcd.print("Date: ");
  printDigits(days);
  lcd.print("/");
  printDigits(months);
  lcd.print("/");
  lcd.print(years);
  lcd.print(cn);
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  printDigits(hours);
  lcd.print(":");
  printDigits(minutes);
  lcd.print(":");
  printDigits(seconds);
  
}

void Gio_bao_thuc() {
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("Bao thuc:");
  lcd.setCursor(0, 1);
  lcd.print("Hour: ");
  printDigits(alarmHour);
  lcd.print(" Minute: ");
  printDigits(alarmMinute);
  lcd.setCursor(15, 1);
  lcd.print(cn);
} 

void chinh_Gio() {
  cnt_BT = 0;
  if(btn){
    if(digitalRead(INC) == LOW){
      if(alarmHour < 23){
        alarmHour++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(alarmHour > 0){
        alarmHour--;
      }
    }
  }
}

void chinh_Phut() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(alarmMinute < 59){
        alarmMinute++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(alarmMinute > 0){
        alarmMinute--;
      }
    }
  }
}

void chinh_TG_Gio() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(hours < 23){
        hours++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(hours > 0){
        hours--;
      }
    }
  }
}

void chinh_TG_Phut() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(minutes < 59){
        minutes++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(minutes > 0){
        minutes--;
      }
    }
  }
}
void chinh_TG_Ngay() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(days < 31){
        days++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(days > 0){
        days--;
      }
    }
  }
}
void chinh_TG_Thang() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(months < 12){
        months++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(months > 0){
        months--;
      }
    }
  }
}
void chinh_TG_Nam() {
  if(btn){
    if(digitalRead(INC) == LOW){
      if(years < 2099){
        years++;
      }
    }
    if(digitalRead(DOWN) == LOW){
      if(years > 0){
        years--;
      }
    }
  }
}

void Baothuc() {
  if(!cn){
    if(hours == alarmHour && minutes == alarmMinute){
      if(stt_btn1){
        digitalWrite(HORN, HIGH);
        cnt_BT++;
      }
      if(cnt_BT >20){
        digitalWrite(HORN, LOW);
      }
      Serial.println(String(cnt_BT));
    }
  }
  
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
        days++;
        if (days == 31)
        {
          months++;
          if (months == 12)
          {
            years++;
            if (years == 2099)
            {
              years=2000;
            }
          }
        } 
      }
    }
  }

  if(btn){
    uint8_t stt_btn = digitalRead(SETUP);
    delay(100);
    if(!stt_btn){
      stt_btn = 1;
      cn++;
    }
  }

  if(cn == 0){
    Hien_dong_ho();
  }
  else if(cn == 1){
    Hien_dong_ho();
    chinh_TG_Gio();
  }
  else if (cn == 2){
    Hien_dong_ho();
    chinh_TG_Phut();
  }
  else if (cn == 3){
    Hien_dong_ho();
    chinh_TG_Ngay();
  }
  else if (cn == 4){
    Hien_dong_ho();
    chinh_TG_Thang();
  }
  else if (cn == 5){
    Hien_dong_ho();
    chinh_TG_Nam();
  }
  else if (cn == 6){
    Gio_bao_thuc();
    chinh_Gio();
  }
  else if(cn == 7){
    Gio_bao_thuc();
    chinh_Phut();
  }
  else if(cn == 8){
    cn = 0;
    Hien_dong_ho();
  }
  //Baothuc();
}