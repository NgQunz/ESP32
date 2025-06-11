
#include <QTRSensors.h>

// QTRSensors
QTRSensors qtr;

// Number of sensors to use
//#define IR 8
//unsigned short qtrValues[IR];

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];


#define Kp 0.2   // 255: 0.1     110: 0.2
#define Ki 0.05    // 255: 0.05    110: 0.05
#define Kd 0.004   // 255: 0.003   110: 0.004

#define rightMaxSpeed 110  // 255  50
#define leftMaxSpeed  110  // 255  50

int SetPoint = (SensorCount - 1) * 500;
 
// Motor-A
int IN1 = 5;
int IN2 = 11;

// Motor-B
int IN3 = 10;
int IN4 = 6;

int lastError=0;
unsigned long cTime, pTime;
float eTime;

float P_error;
float I_error;
float D_error;
float PID_value;

void setup() {
  Serial.begin(9600);  
  qtr.setTypeAnalog();

  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  qtr.setSensorPins((const uint8_t[]){A7, A6, A5, A4, A3, A2, A1, A0},SensorCount);
  qtr.setEmitterPin(2);

  for (uint8_t i = 0; i < 250; i++){
    if (i % 5 == 0) {
      digitalWrite(4, !digitalRead(4));
      digitalWrite(8, !digitalRead(8));
    }
    qtr.calibrate();
    delay(20);
  }

  pinMode(IN1, OUTPUT);   
  pinMode(IN2, OUTPUT);   
  pinMode(IN3, OUTPUT);   
  pinMode(IN4, OUTPUT); 

  digitalWrite(4, LOW);
  digitalWrite(8, LOW);

  for (int i = 0; i < 400; i++){
    if (i % 5 == 0) {
      digitalWrite(7, !digitalRead(7));
    }
    delay(20);
  }
}

void run_fwd(int valueSA, int valueSB){
  // Motor A
    analogWrite(IN1, 0);
    analogWrite(IN2, valueSA);  
    analogWrite(IN3, valueSB);
    analogWrite(IN4, 0);  
}

void LEDs(void){ 
  if (abs(P_error) < 150) digitalWrite(7, HIGH);
  else digitalWrite(7, LOW);
  if (P_error > 130) digitalWrite(8, HIGH);
  else digitalWrite(8, LOW);
  if (P_error < -130) digitalWrite(4, HIGH);
  else digitalWrite(4, LOW);

}

void Run_robot(){
  char pntX[100];
  char floX[10];
  
  int med_Speed_R;
  int med_Speed_L;

  int position = qtr.readLineWhite(sensorValues);
 // Serial.println(position);
  P_error = position - SetPoint;

  cTime = millis();
  eTime = (float)(cTime - pTime) / 1000;

  I_error = I_error * 2 / 3 + P_error * eTime;
  D_error = (P_error - lastError) / eTime;
  PID_value = Kp * P_error + Ki * I_error + Kd * D_error;

  
  lastError = P_error;
  pTime = cTime;

  med_Speed_L = leftMaxSpeed - abs(PID_value);
  med_Speed_R = rightMaxSpeed - abs(PID_value);

  int leftMotorSpeed = med_Speed_L + PID_value;
  int rightMotorSpeed = med_Speed_R - PID_value;

  leftMotorSpeed = constrain(leftMotorSpeed, 0, leftMaxSpeed);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, rightMaxSpeed);


  Serial.print(rightMotorSpeed);
  Serial.print("   ");
  Serial.println(leftMotorSpeed);

  run_fwd(leftMotorSpeed, rightMotorSpeed);
  delayMicroseconds(140);

}

void loop() {
  Run_robot();
  LEDs();
}