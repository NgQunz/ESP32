#include <Arduino.h>
#include "BluetoothSerial.h"

#include "config.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "maze.h"

extern BluetoothSerial SerialBT;

static void printBoth(const String &msg)
{
  Serial.println(msg);
  SerialBT.println(msg);
}

static void printTofBT()
{
  SerialBT.print("L:");
  SerialBT.print(LeftMM);
  SerialBT.print(" F:");
  SerialBT.print(FrontMM);
  SerialBT.print(" R:");
  SerialBT.println(RightMM);
}

void setup()
{
  Serial.begin(115200);
  SerialBT.begin(DEVICE_NAME);

  initI2C();
  initMotors();
  initEncoders();

  initMPU6050();
  calibrateGyroZ();
  resetYaw();

  setupTOFAddresses();
  readTof();

  initMazeScore();

  printBoth("Micromouse ready");
  printBoth("CELL_PULSE = " + String(CELL_PULSE));
  printBoth("MM_PER_PULSE = " + String(MM_PER_PULSE, 4));
}

void loop()
{
  updateYaw();

  if (SerialBT.available())
  {
    char c = SerialBT.read();

    if (c == 'f')
    {
      printBoth("CMD: goCell");
      goCell();
    }
    else if (c == 'l')
    {
      printBoth("CMD: turnLeft90");
      turnLeft90();
    }
    else if (c == 'r')
    {
      printBoth("CMD: turnRight90");
      turnRight90();
    }
    else if (c == 'b')
    {
      printBoth("CMD: turnBack180");
      turnBack180();
    }
    else if (c == 't')
    {
      readTof();
      printTofBT();
    }
    else if (c == 'm')
    {
      // Chạy 1 bước think() thủ công (debug)
      think();
    }
    else if (c == 'a')
    {
      // Chạy tự động toàn bộ mê cung đến goal
      // Gửi 's' qua Bluetooth bất kỳ lúc nào để dừng khẩn cấp
      printBoth("CMD: runMaze start (send 's' to stop)");
      initMazeScore();
      runMaze();
      if (goalReached)
        printBoth("GOAL REACHED!");
      else
        printBoth("runMaze stopped");
    }
    else if (c == 'd')
    {
      printBoth("DEBUG goCell start");
      resetEncoders();
      float targetYaw = yawDeg;
      unsigned long startMs = millis();
      unsigned long lastLog = 0;

      while (avgAbsEncoder() < CELL_PULSE)
      {
        if (millis() - startMs > 3000)
          break;
        updateYaw();
        readTof();

        long encError = encoder2Count - encoder1Count;
        float yawError = targetYaw - yawDeg;
        int encCorrection = (int)(KP_ENC * encError);
        int gyroCorrection = (int)(KP_GYRO * yawError);
        int wallCorrection = 0;
        if (LeftMM < SIDE_WALL_USE_MM && RightMM < SIDE_WALL_USE_MM)
          wallCorrection = (int)(KP_WALL * (LeftMM - RightMM));

        int leftPWM = BASE_PWM_FORWARD - encCorrection + gyroCorrection + wallCorrection;
        int rightPWM = BASE_PWM_FORWARD + encCorrection - gyroCorrection - wallCorrection;
        leftPWM = constrain(leftPWM, MIN_PWM_FORWARD, MAX_PWM_FORWARD);
        rightPWM = constrain(rightPWM, MIN_PWM_FORWARD, MAX_PWM_FORWARD);
        setMotor(leftPWM, rightPWM);

        if (millis() - lastLog >= 100)
        {
          SerialBT.println(
              "E1=" + String(encoder1Count) + " E2=" + String(encoder2Count) +
              " encErr=" + String(encError) +
              " encC=" + String(encCorrection) +
              " gyroC=" + String(gyroCorrection) +
              " wallC=" + String(wallCorrection) +
              " L=" + String(leftPWM) + " R=" + String(rightPWM));
          lastLog = millis();
        }
      }
      brakeStop();
      printBoth("DEBUG done E1=" + String(encoder1Count) + " E2=" + String(encoder2Count));
    }
    else if (c == 's')
    {
      stopAllMotors();
      printBoth("CMD: stop");
    }
    else if (c == 'i')
    {
      // Reset lại toàn bộ vị trí & bản đồ
      initMazeScore();
      resetYaw();
      printBoth("CMD: reset maze & yaw");
    }
    else if (c == 'y')
    {
      // In yaw liên tục 5 giây để test
      unsigned long start = millis();
      while (millis() - start < 50000)
      {
        updateYaw();
        SerialBT.println(String(yawDeg, 2));
        delay(100);
      }
    }
  }

  // In trạng thái định kỳ
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 300)
  {
    readTof();

    String msg = "X=" + String(x) +
                 " Y=" + String(y) +
                 " F=" + String(facing) +
                 " | E1=" + String(encoder1Count) +
                 " E2=" + String(encoder2Count) +
                 " | Yaw=" + String(yawDeg, 2) +
                 " | L=" + String(LeftMM) +
                 " F=" + String(FrontMM) +
                 " R=" + String(RightMM);

    printBoth(msg);
    lastPrint = millis();
  }
}