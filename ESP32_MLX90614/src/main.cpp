#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()
{
  Serial.begin(9600);
  mlx.begin();
}

void loop()
{

  Serial.print("*CtObject = ");
  Serial.print(mlx.readObjectTempC());
  Serial.println("*C");
  Serial.print("Ambient = ");
  Serial.print(mlx.readAmbientTempF());
  Serial.print("*FtObject = ");
  Serial.print(mlx.readObjectTempF());
  Serial.println("*F");
  Serial.println();
  delay(500);
}

// #include <Wire.h>
// #include <MAX30105.h>
// #include <heartRate.h>
// #include <spo2_algorithm.h>

// MAX30105 particleSensor;

// // Buffer cho SpO₂
// #define BUFFER_SIZE 100
// uint32_t irBuffer[BUFFER_SIZE];
// uint32_t redBuffer[BUFFER_SIZE];
// int32_t bufferLength;
// int32_t spo2;
// int8_t validSPO2;
// int32_t heartRate;
// int8_t validHeartRate;

// // Bộ lọc trung bình BPM
// const byte RATE_SIZE = 4;
// byte rates[RATE_SIZE];
// byte rateSpot = 0;
// int beatAvg = 0;

// long lastBeat = 0;
// float beatsPerMinute;

// void setup()
// {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Initializing...");

//   Wire.begin(21, 22);

//   if (!particleSensor.begin(Wire, 100000))
//   {
//     Serial.println(F("MAX30105/MAX30102 not found. Check wiring/power."));
//     while (1)
//       ;
//   }

//   // Cấu hình cảm biến
//   byte ledBrightness = 60; // 0-255
//   byte sampleAverage = 4;
//   byte ledMode = 2;     // Red + IR
//   byte sampleRate = 50; // Hz (giảm để ổn định hơn)
//   int pulseWidth = 411;
//   int adcRange = 4096;

//   particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
//   particleSensor.setPulseAmplitudeGreen(0);

//   Serial.println(F("Place your finger on the sensor..."));
// }

// void loop()
// {
//   bufferLength = BUFFER_SIZE;

//   // Thu thập dữ liệu
//   for (byte i = 0; i < bufferLength; i++)
//   {
//     while (!particleSensor.available())
//       particleSensor.check();

//     redBuffer[i] = particleSensor.getRed();
//     irBuffer[i] = particleSensor.getIR();
//     particleSensor.nextSample();
//   }

//   // Tính toán ban đầu
//   maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
//                                          &spo2, &validSPO2, &heartRate, &validHeartRate);

//   // Vòng lặp chính
//   while (1)
//   {
//     // Dời buffer
//     for (byte i = 25; i < 100; i++)
//     {
//       redBuffer[i - 25] = redBuffer[i];
//       irBuffer[i - 25] = irBuffer[i];
//     }

//     // Đọc thêm mẫu mới
//     for (byte i = 75; i < 100; i++)
//     {
//       while (!particleSensor.available())
//         particleSensor.check();

//       redBuffer[i] = particleSensor.getRed();
//       irBuffer[i] = particleSensor.getIR();
//       particleSensor.nextSample();

//       // Tính BPM bằng hàm checkForBeat()
//       if (checkForBeat(irBuffer[i]) == true)
//       {
//         long delta = millis() - lastBeat;
//         lastBeat = millis();

//         beatsPerMinute = 60 / (delta / 1000.0);

//         if (beatsPerMinute > 40 && beatsPerMinute < 180) // lọc giá trị bất thường
//         {
//           rates[rateSpot++] = (byte)beatsPerMinute;
//           rateSpot %= RATE_SIZE;

//           beatAvg = 0;
//           for (byte x = 0; x < RATE_SIZE; x++)
//             beatAvg += rates[x];
//           beatAvg /= RATE_SIZE;
//         }
//       }
//     }

//     // Cập nhật SpO₂ và nhịp tim
//     maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
//                                            &spo2, &validSPO2, &heartRate, &validHeartRate);

//     // Xuất kết quả
//     Serial.print("SpO₂ = ");
//     if (validSPO2 && spo2 > 70 && spo2 <= 100)
//       Serial.print(spo2);
//     else
//       Serial.print("N/A");

//     Serial.print(" %,  BPM = ");
//     if (beatAvg > 40 && beatAvg < 180)
//       Serial.print(beatAvg);
//     else
//       Serial.print("N/A");

//     Serial.println();
//   }
// }

#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <spo2_algorithm.h>

MAX30105 particleSensor;

// --- Biến đo BPM ---
long lastBeat = 0;
float beatsPerMinute;
float sumBPM = 0;
int countBPM = 0;

// --- Biến đo SpO₂ ---
#define BUFFER_SIZE 100
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];
int32_t bufferLength;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

// --- Quản lý trạng thái ---
unsigned long startTime;
bool measuringBPM = true; // true = đang đo BPM, false = đang đo SpO₂

void setup()
{
  Serial.begin(115200);
  Serial.println("Khoi dong cam bien MAX30102...");

  Wire.begin(21, 22); // SDA=21, SCL=22 cho ESP32

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("Khong tim thay MAX30102. Kiem tra ket noi!");
    while (1)
      ;
  }

  // Cấu hình cảm biến
  byte ledBrightness = 60; // 0-255
  byte sampleAverage = 4;
  byte ledMode = 2;     // Red + IR
  byte sampleRate = 50; // Hz
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  particleSensor.setPulseAmplitudeGreen(0);

  startTime = millis();
  Serial.println("Dat ngon tay len cam bien...");
}

// --- Hàm đo BPM trong 5 giây ---
void BPM_CAL()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255)
    {
      sumBPM += beatsPerMinute;
      countBPM++;
    }
  }

  // Sau 5s thì in kết quả và chuyển sang đo SpO₂
  if (millis() - startTime >= 5000)
  {
    if (countBPM > 0)
    {
      float avgBPM = sumBPM / countBPM;
      Serial.print("Trung binh BPM sau 5s = ");
      Serial.println(avgBPM);
    }
    else
    {
      Serial.println("Khong phat hien nhip tim hop le trong 5s!");
    }

    // Reset để chuẩn bị đo SpO₂
    sumBPM = 0;
    countBPM = 0;
    measuringBPM = false;
    startTime = millis();
  }
}

// --- Hàm đo SpO₂ ---
void SPO2_CAL()
{
  bufferLength = BUFFER_SIZE;

  // Thu thập dữ liệu ban đầu
  for (byte i = 0; i < bufferLength; i++)
  {
    while (!particleSensor.available())
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  // Tính toán SpO₂ lần đầu
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
                                         &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Dời buffer và cập nhật mỗi vòng loop
  for (byte i = 25; i < 100; i++)
  {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  for (byte i = 75; i < 100; i++)
  {
    while (!particleSensor.available())
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
                                         &spo2, &validSPO2, &heartRate, &validHeartRate);

  Serial.print("SpO₂ = ");
  if (validSPO2 && spo2 > 70 && spo2 <= 100)
    Serial.print(spo2);
  else
    Serial.print("N/A");
  Serial.println(" %");

  // Sau 10s đo SpO₂ thì quay lại đo BPM
  if (millis() - startTime >= 10000)
  {
    measuringBPM = true;
    startTime = millis();
  }
}

void loop()
{
  if (measuringBPM)
  {
    BPM_CAL();
  }
  else
  {
    SPO2_CAL();
  }
}
