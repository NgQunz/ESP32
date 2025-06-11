
#include "Motion.h"
#define ENCODER_1A 34
#define ENCODER_1B 35
#define ENCODER_2A 32
#define ENCODER_2B 33

// Chân điều khiển động cơ (DRV8833)

#define Sleep 4
#define left1 23
#define left2 19
#define right1 18
#define right2 5

volatile int encoder_count_1 = 0;
volatile int encoder_count_2 = 0;
const int pulses_per_cm = 28;
const int distance_cm = 18;
const int target_pulses = distance_cm * pulses_per_cm;

void IRAM_ATTR encoder1ISR()
{
  encoder_count_1++;
}

void IRAM_ATTR encoder2ISR()
{
  encoder_count_2++;
}

void Motion::set_pin()
{

  // set left mt left:
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);

  // set pin mt right:
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(Sleep, INPUT_PULLUP);
  pinMode(ENCODER_1A, INPUT);
  pinMode(ENCODER_1B, INPUT);
  pinMode(ENCODER_2A, INPUT);
  pinMode(ENCODER_2B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_1A), encoder1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2B), encoder2ISR, RISING);
  // set standby pin
  pinMode(5, OUTPUT);
}

void Motion::stop()
{
  digitalWrite(Sleep, 0);
  analogWrite(left1, 0);
  analogWrite(left2, 0);
  analogWrite(right1, 0);
  analogWrite(right2, 0);
  Serial.println("Stop");
  delay(400);
}

void Motion::LUI()
{
  analogWrite(left1, 80);
  analogWrite(left2, 0);
  analogWrite(right1, 80);
  analogWrite(right2, 0);
  Serial.println("lui!");
  delay(450);
}
void Motion::run18cm()
{
  encoder_count_1 = 0;
  encoder_count_2 = 0;

  while (encoder_count_2 < target_pulses)
  {
    int error = encoder_count_1 - encoder_count_2;
    int speed1 = speed_max;
    int speed2 = speed_max;

    analogWrite(left1, 0);
    analogWrite(left2, speed1);
    analogWrite(right1, 0);
    analogWrite(right2, speed2);
    Serial.println(encoder_count_2);
  }
  Serial.println("chay!");
}
void Motion::T()
{
  int ok = 1;
  int speed1 = speed_max - 10;
  while (ok)
  {
    if (wallLeft() == 1 && wallRight() == 0)
    {
      analogWrite(left1, 0);
      analogWrite(left2, speed1);
      analogWrite(right1, speed1);
      analogWrite(right2, 0);
    }
    else if (wallLeft() == 0 && wallRight() == 1)
    {
      analogWrite(left1, speed1);
      analogWrite(left2, 0);
      analogWrite(right1, 0);
      analogWrite(right2, speed1);
    }
    else if (wallFront() == 1)
    {
      ok = 0;
    }
    else
    {
      analogWrite(left1, 0);
      analogWrite(left2, 100);
      analogWrite(right1, 0);
      analogWrite(right2, 100);
    }
  }
  if (ok == 0)
  {
    Motion::stop();
  }
}

void Motion::TLUI()
{
  analogWrite(left1, 80);
  analogWrite(left2, 0);
  analogWrite(right1, 80);
  analogWrite(right2, 0);
  delay(150);
}
void Motion::TRAI()
{
  // unsigned long startTime = millis();

  // while (millis() - startTime < duration) {
  analogWrite(left1, 100);
  analogWrite(left2, 0);
  analogWrite(right1, 0);
  analogWrite(right2, 100);
  Serial.println("trai!");
  delay(160);

  // }

  // stop();  // Dừng xe sau khi hết thời gian
}

void Motion::PHAI()
{
  analogWrite(left1, 0);
  analogWrite(left2, 100);
  analogWrite(right1, 100);
  analogWrite(right2, 0);
  Serial.println("phai!");
  delay(160);
}

void Motion::back()
{
  digitalWrite(5, 1);
  long t = millis();
  while ((millis() - t) <= 2000)
  {
    analogWrite(left1, 200);
    analogWrite(left2, 0);
    analogWrite(right1, 200);
    analogWrite(right2, 0);
  }
  Motion::stop();
  delay(10000);
}
void Motion::set_target(float first_read_from_imu)
{
  target = first_read_from_imu;
}

float Motion::get_target()
{
  return target;
}

void Motion::speed_increase()
{

  if (speed_tb >= speed_target)
    speed_tb += 1;
}

void Motion::set_error(float now)
{
  //  error = (now - target) * 100  ; // imu

  error = now - target; // tof
}

float Motion::get_error()
{
  return error;
}

void Motion::set_speed(int lsp, int rsp)
{
  //  speed_increase();

  if (rsp < 0)
  {
    analogWrite(right1, -rsp);
    analogWrite(right2, 0);
  }
  else
  {
    analogWrite(right1, 0);
    analogWrite(right2, rsp + 20);
  }
  if (lsp < 0)
  {
    analogWrite(left1, -lsp);
    analogWrite(left2, 0);
  }
  else
  {
    analogWrite(left1, 0);
    analogWrite(left2, lsp);
  }
}

void Motion::run_straight()
{
  analogWrite(left1, 0);
  analogWrite(left2, 70);
  analogWrite(right1, 0);
  analogWrite(right2, 70);
}
void Motion::set_state(int state)
{

  if (state)
    digitalWrite(5, HIGH);

  else
    digitalWrite(5, LOW);
}

void Motion::set_speed_tb(int s_tb)
{
  speed_tb = s_tb;
}
void Motion::run_pid()
{

  // speed_increase();
  int lsp, rsp;
  P = error;
  I = error + I;
  D = error - previous_error;

  float PIDvalue = Pvalue * P + Dvalue * D + Ivalue * I;

  previous_error = error;
  lsp = speed_tb + PIDvalue;
  rsp = speed_tb - PIDvalue;
  //
  //   } else{
  //       lsp = speed_tb - PIDvalue ;
  //       rsp = speed_tb + PIDvalue;
  //   }
  //   lsp = speedmax + PIDvalue;
  //   rsp = speedmax - PIDvalue;

  if (lsp > speed_max)
  {
    lsp = speed_max;
  }
  else if (lsp < -speed_max)
  {
    lsp = -speed_max;
  }
  if (rsp > speed_max)
  {
    rsp = speed_max;
  }
  else if (rsp < -speed_max)
  {
    rsp = -speed_max;
  }
  set_speed(lsp, rsp);
}
