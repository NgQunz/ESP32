#include <Arduino.h>

// ===== GP2Y1010AU0F Pins =====
#define GP2Y_LED 13 // LED driver pin
#define GP2Y_ADC 33 // Analog input pin (ESP32 ADC)

// ===== GP2Y Timings =====
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

// ===== Variables =====
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void GP2Y_setup()
{
  pinMode(GP2Y_LED, OUTPUT);
  digitalWrite(GP2Y_LED, HIGH); // LED OFF (active LOW)
}

// ===== Hàm đọc bụi GP2Y =====
// Trả về: µg/m³
float Run_GP2Y()
{
  long sum = 0;

  for (int i = 0; i < 100; i++)
  {
    digitalWrite(GP2Y_LED, LOW);
    delayMicroseconds(samplingTime);

    voMeasured = analogRead(GP2Y_ADC);

    delayMicroseconds(deltaTime);
    digitalWrite(GP2Y_LED, HIGH);
    delayMicroseconds(sleepTime);

    calcVoltage = voMeasured * (5.0 / 1024.0); // nếu ESP32 thì đổi 1024 → 4095 và 5.0 → 3.3
    dustDensity = 0.17 * calcVoltage - 0.1;

    sum += dustDensity;
    delay(100);
  }

  float avgDust = (float)sum / 100.0; // mg/m³
  float ugm3 = avgDust * 1000.0;      // μg/m³

  return ugm3;
}

void setup()
{
  Serial.begin(9600);
  GP2Y_setup();

  Serial.println("GP2Y1010AU0F Dust Sensor Started");
}

void loop()
{
  float dust_ug = Run_GP2Y();

  Serial.print("Dust: ");
  Serial.print(dust_ug, 1);
  Serial.println(" ug/m3");

  delay(2000);
}
