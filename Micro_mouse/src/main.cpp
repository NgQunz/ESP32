#include "FastIMU.h"
#include "Madgwick.h"
#include <Wire.h>
#include <BluetoothSerial.h>
#include <vl53lx.h>
#include <Motion.h>

#define IMU_ADDRESS 0x68    // Change to the address of the IMU
#define PERFORM_CALIBRATION // Comment to disable startup calibration
MPU6050 IMU;                // Change to the name of any supported IMU!

Motion motion;

calData calib = {0}; // Calibration data
AccelData IMUAccel;  // Sensor data
GyroData IMUGyro;
MagData IMUMag;
Madgwick filter;
BluetoothSerial SerialBT;

void get()
{
  IMU.update();
  IMU.getAccel(&IMUAccel);
  IMU.getGyro(&IMUGyro);
  filter.updateIMU(IMUGyro.gyroX, IMUGyro.gyroY, IMUGyro.gyroZ, IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ);
}

String s = " ";
int t = 0;
int corner1 = 0;

// ========================================= set up function ======================================

void setup()
{

  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000);

  motion.set_pin();
  vl53lx_setup();

  motion.stop();
}

//============================ end of setup ========================================
// time left 140ms, right 160ms
void loop()
{
  motion.stop();
  delay(3000);

  // for (int i = 0; i < 2; i++)
  // {
  motion.T();
  motion.stop();
  // }
  motion.PHAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  motion.TLUI();
  motion.stop();
  // for (int i = 0; i < 2; i++)
  // {
  motion.T();
  motion.stop();
  // }
  motion.PHAI();
  motion.stop();
  motion.run18cm();
  motion.stop();
  motion.TRAI();
  motion.stop();
  motion.run18cm();
  motion.stop();
  motion.PHAI();
  motion.stop();
  motion.T();
  motion.stop();
  motion.TRAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 12; i++)
  // {
  motion.T();
  motion.stop();
  // }
  motion.TRAI();
  motion.stop();
  // motion.TRAI();
  // motion.stop();
  motion.T();
  motion.stop();
  motion.TRAI();
  motion.stop();
  motion.LUI();
  // for (int i = 0; i < 11; i++)
  // {
  motion.T();
  motion.stop();
  // }
  motion.PHAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 5; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  for (int i = 0; i < 2; i++)
  {
    motion.run18cm();
    motion.stop();
  }
  motion.TLUI();
  motion.stop();
  motion.PHAI();
  motion.stop();

  motion.T();
  motion.stop();

  motion.PHAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 3; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.PHAI();
  motion.stop();

  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 5; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();
  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 9; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();

  motion.T();
  motion.stop();

  motion.TRAI();
  motion.stop();
  // for (int i = 0; i < 8; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.PHAI();
  motion.stop();
  motion.T();
  motion.stop();

  motion.PHAI();
  motion.stop();

  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 8; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();
  // for (int i = 0; i < 3; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();

  motion.LUI();
  motion.stop();
  // for (int i = 0; i < 5; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.PHAI();
  motion.stop();
  // for (int i = 0; i < 2; i++)
  // {
  motion.T();
  motion.stop();
  // }

  motion.TRAI();
  motion.stop();

  motion.LUI();
  motion.stop();

  motion.T();
  motion.stop();

  motion.TRAI();
  motion.stop();
  // for (int i = 0; i < 2; i++)
  // {
  motion.T();
  motion.stop();
  // }
}
// #include <Arduino.h>
// #define ENCODER_PIN 33
// volatile int encoder_count = 0;

// void IRAM_ATTR handleInterrupt()
// {
//   encoder_count++; // Tăng biến đếm mỗi khi có xung
// }

// void setup()
// {
//   Serial.begin(9600);
//   pinMode(ENCODER_PIN, INPUT);
//   attachInterrupt(ENCODER_PIN, handleInterrupt, RISING); // Kích hoạt ngắt khi có xung lên
// }

// void loop()
// {
//   Serial.print("Encoder count: ");
//   Serial.println(encoder_count);
//   delay(100); // Hiển thị giá trị mỗi 500ms
// }
