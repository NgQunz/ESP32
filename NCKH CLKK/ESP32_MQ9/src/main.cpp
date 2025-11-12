#include <Arduino.h>

const int MQ_PIN = 35;                // ADC pin
const float RL = 10000.0;             // Load resistor (10k typical)
const float SENSOR_VCC = 5.0;         // Sensor supply voltage
const float DIVIDER_CORRECTION = 1.0; // Nếu có mạch chia, chỉnh lại

const float ADC_MAX = 4095.0;
const float ADC_REF_V = 3.3;

// Hệ số đường cong cho CO (MQ-9)
const float M_CO = -0.48;
const float B_CO = 0.35;

float R0 = 10000.0; // Gán giá trị R0 sau khi hiệu chuẩn

int samplesForAverage = 10; // trung bình trong 1 lần đọc ADC
int calibSamples = 200;     // số mẫu khi calibrate
int readCount = 10;         // số lần đo để lấy trung bình cuối cùng

// ---------- Helpers ----------
float readAnalogVoltage()
{
  uint32_t sum = 0;
  for (int i = 0; i < samplesForAverage; i++)
  {
    sum += analogRead(MQ_PIN);
    delay(10);
  }
  float adc_raw = (float)sum / samplesForAverage;
  float measuredV = (adc_raw / ADC_MAX) * ADC_REF_V;
  return measuredV * DIVIDER_CORRECTION;
}

float calcRs(float vout)
{
  if (vout <= 0.0001)
    return 1e6;
  return RL * (SENSOR_VCC - vout) / vout;
}

float RsRo_to_ppm_CO(float rs_ro)
{
  if (rs_ro <= 0)
    return -1;
  float log_rsro = log10(rs_ro);
  float log_ppm = (log_rsro - B_CO) / M_CO;
  return pow(10, log_ppm);
}

float RsRoRatio(float Rs) { return Rs / R0; }

// ---------- Calibration ----------
void calibrateR0()
{
  Serial.println("== CALIBRATION START ==");
  Serial.println("Đặt cảm biến trong không khí sạch...");
  float sumRs = 0;
  for (int i = 0; i < calibSamples; i++)
  {
    float v = readAnalogVoltage();
    float rs = calcRs(v);
    sumRs += rs;
    if (i % 50 == 0)
      Serial.print(".");
    delay(200);
  }
  float avgRs = sumRs / calibSamples;
  Serial.println();
  Serial.print("Average Rs = ");
  Serial.print(avgRs);
  Serial.print(" Ω → R0 = ");
  Serial.println(avgRs);
  Serial.println("== CALIBRATION END ==");
}

// ---------- Setup/Loop ----------
void setup()
{
  Serial.begin(9600);
  delay(2000);
  analogSetPinAttenuation(MQ_PIN, ADC_11db);
  pinMode(MQ_PIN, INPUT);
  Serial.println("MQ-9 CO monitor - ESP32");
  delay(1000);
  // calibrateR0(); // bật khi muốn hiệu chuẩn
}

void loop()
{
  float sumVout = 0, sumRs = 0, sumRatio = 0, sumPPM = 0;

  for (int i = 0; i < readCount; i++)
  {
    float vout = readAnalogVoltage();
    float Rs = calcRs(vout);
    float ratio = RsRoRatio(Rs);
    float co_ppm = RsRo_to_ppm_CO(ratio);

    sumVout += vout;
    sumRs += Rs;
    sumRatio += ratio;
    if (co_ppm > 0 && isfinite(co_ppm))
      sumPPM += co_ppm;

    delay(500);
  }

  float avgVout = sumVout / readCount;
  float avgRs = sumRs / readCount;
  float avgRatio = sumRatio / readCount;
  float avgPPM = sumPPM / readCount;

  Serial.print("Vout: ");
  Serial.print(avgVout, 3);
  Serial.print(" V | Rs: ");
  Serial.print(avgRs, 1);
  Serial.print(" Ω | Rs/R0: ");
  Serial.print(avgRatio, 3);
  Serial.print(" | CO: ");
  if (avgPPM > 0 && isfinite(avgPPM))
  {
    Serial.print(avgPPM, 2);
    Serial.println(" ppm");
  }
  else
  {
    Serial.println("N/A");
  }

  delay(1000);
}
