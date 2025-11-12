// #include <Arduino.h>

// // ==== MPS20N + AD620 ====
// #define SENSOR_PIN 34
// #define VREF 3.3
// #define GAIN 5.5 // hệ số khuếch đại thực tế
// #define VCC 4.6
// #define SENS 0.2e-3 // V/V/Pa

// // ==== MX1508 ====
// #define IN1 16
// #define IN2 17
// #define IN3 25
// #define IN4 33
// #define CH_MOTOR 0
// #define CH_VALVE 1

// // ==== Ngưỡng áp suất (kPa) ====
// #define P_LOW 5.0   // bật bơm khi < 5 kPa
// #define P_HIGH 10.0 // tắt bơm khi > 10 kPa

// // ==== Hàm motor & valve ====
// void motorRun(uint8_t pwm)
// {
//   ledcWrite(CH_MOTOR, pwm);
//   digitalWrite(IN2, LOW);
// }

// void motorStop()
// {
//   ledcWrite(CH_MOTOR, 0);
//   digitalWrite(IN2, LOW);
// }

// void valveOn()
// {
//   ledcWrite(CH_VALVE, 85); // duty ~3V từ 9V
//   digitalWrite(IN4, LOW);
// }

// void valveOff()
// {
//   ledcWrite(CH_VALVE, 0);
//   digitalWrite(IN4, LOW);
// }

// void setup()
// {
//   Serial.begin(9600);

//   // PWM setup
//   ledcSetup(CH_MOTOR, 1000, 8);
//   ledcSetup(CH_VALVE, 1000, 8);
//   ledcAttachPin(IN1, CH_MOTOR);
//   ledcAttachPin(IN3, CH_VALVE);

//   pinMode(IN2, OUTPUT);
//   pinMode(IN4, OUTPUT);

//   motorStop();
//   valveOff();
// }

// void loop()
// {
//   // === Đọc áp suất ===
//   int raw = analogRead(SENSOR_PIN);
//   float v_in = (raw / 4095.0) * VREF; // điện áp đo được tại ESP32
//   float v_sensor = v_in / GAIN;       // điện áp gốc trước khuếch đại
//   float pressure_pa = v_sensor / (SENS * VCC);
//   float pressure_kpa = pressure_pa / 1000.0;

//   Serial.print("Pressure: ");
//   Serial.print(pressure_kpa, 2);
//   Serial.println(" kPa");

//   // motorRun(225); // bật bơm ~70% duty
//   delay(1000);
// }

/*
Pressure Measurements with the
MPS20N0040D Breakout Board
with the HX710B/HX711 ADC
5V Supply Voltage
 */
#include <Q2HX711.h>

const byte MPS_OUT_pin = 34; // OUT data pin
const byte MPS_SCK_pin = 14; // clock data pin
int avg_size = 10;           // #pts to average over

Q2HX711 MPS20N0040D(MPS_OUT_pin, MPS_SCK_pin); // start comm with the HX710B

void setup()
{
  Serial.begin(9600); // start the serial port
}

void loop()
{
  float avg_val = 0.0; // variable for averaging
  for (int ii = 0; ii < avg_size; ii++)
  {
    avg_val += MPS20N0040D.read(); // add multiple ADC readings
    delay(50);                     // delay between readings
  }
  avg_val /= avg_size;
  Serial.println(avg_val, 0); // print out the average
}