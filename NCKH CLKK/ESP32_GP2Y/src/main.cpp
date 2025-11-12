#include <Arduino.h>

int measurePin = 33;
int ledPower = 13;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);
}

void loop()
{
  float sum = 0;

  // Đo 10 lần rồi tính trung bình
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(ledPower, LOW);
    delayMicroseconds(samplingTime);
    sum += analogRead(measurePin);
    delayMicroseconds(deltaTime);
    digitalWrite(ledPower, HIGH);
    delayMicroseconds(sleepTime);
    delay(100);
  }

  voMeasured = sum / 100.0; // giá trị trung bình ADC

  // Nếu ESP32: 12-bit ADC → chia 4095, nếu Arduino: 10-bit → chia 1024
  calcVoltage = voMeasured * (5.0 / 1024.0);

  dustDensity = 0.17 * calcVoltage - 0.1; // mg/m³
  // if (dustDensity > 0)
  // {
  float dustDensity_ug = dustDensity * 1000; // µg/m³
                                             // if (dustDensity > 0)
                                             // {

  Serial.print("ADC Avg: ");
  Serial.print(voMeasured);
  Serial.print(" | Voltage: ");
  Serial.print(calcVoltage, 3);
  Serial.print(" V | Dust: ");
  Serial.print(dustDensity, 3);
  Serial.print(" mg/m³ (");
  Serial.print(dustDensity_ug, 1);
  Serial.println(" µg/m³)");
  delay(1000);
  //}
}
// #include <Arduino.h>

// int measurePin = 27;
// int ledPower = 13;

// int samplingTime = 280;
// int deltaTime = 40;
// int sleepTime = 9680;

// float voMeasured = 0;
// float calcVoltage = 0;
// float dustDensity = 0;

// void setup()
// {
//   Serial.begin(9600);
//   pinMode(ledPower, OUTPUT);
// }

// void loop()
// {
//   float sumDust = 0;

//   for (int i = 0; i < 10; i++)
//   {
//     digitalWrite(ledPower, LOW);
//     delayMicroseconds(samplingTime);
//     voMeasured = analogRead(measurePin);
//     delayMicroseconds(deltaTime);
//     digitalWrite(ledPower, HIGH);
//     delayMicroseconds(sleepTime);

//     calcVoltage = voMeasured * (5.0 / 1024.0);
//     dustDensity = 0.17 * calcVoltage - 0.1;

//     sumDust += dustDensity;
//     delay(100);
//   }

//   float avgDust = sumDust / 10.0;

//   Serial.print("Dust Density Avg (mg/m3): ");
//   Serial.println(avgDust);
//   delay(1000);
// }
