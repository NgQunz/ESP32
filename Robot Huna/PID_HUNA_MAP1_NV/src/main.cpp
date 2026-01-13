#include <Arduino.h>
#include "MotorDriver.h"
#include "PIDLineFollower.h"
#include "IRSensorArray.h"
#include "BuzzerController.h"
#include "PinConfig.h"
#include "Arm_Servo.h"

// ========== SENSOR PINS ARRAY ==========
const uint8_t sensorPins[NUM_SENSORS] = {SENSOR_0, SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4};

// ========== OBJECTS ==========
MotorDriver motor(ML_DIR, ML_PWM, MR_DIR, MR_PWM);
IRSensorArray sensor(sensorPins, NUM_SENSORS);
PIDLineFollower pid(motor, sensor);
BuzzerController buzzer(BUZZER_PIN);

// ========== STATE MACHINE ==========
enum RobotState
{
  STATE_IDLE,
  STATE_CALIB_WHITE,
  STATE_CALIB_BLACK,
  STATE_READY,
  STATE_MISSION_RUNNING
};

RobotState currentState = STATE_IDLE;
int buttonPressCount = 0;

// Button debounce
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Junction detection
const unsigned long JUNCTION_COOLDOWN = 500;
unsigned long lastJunctionTime = 0; // ✅ CHUYỂN LÊN ĐÂY (global)

// ========== LED CONTROL FUNCTIONS ==========
void ledSteady()
{
  digitalWrite(LED_PIN, HIGH);
}

void ledOff()
{
  digitalWrite(LED_PIN, LOW);
}

void ledBlinkSlow(unsigned long duration)
{
  unsigned long startTime = millis();

  while (millis() - startTime < duration)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

// ========== CALIBRATION WITH LED ==========
void calibrateWhiteWithLED()
{
  Serial.println(F("\n=== CALIB TRANG ==="));
  Serial.println(F("Dat tren nen trang..."));
  Serial.println(F("Bat dau sau 2 giay...\n"));

  ledBlinkSlow(2000);

  Serial.println(F("Dang calib... LED nhay nhanh"));

  const int samples = 50;
  const int delayMs = 10;

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensor._whiteValues[i] = 0;
  }

  Serial.println(F("Reading 50 samples..."));
  Serial.println(F("Sample | S0    S1    S2    S3    S4"));
  Serial.println(F("-------|---------------------------"));

  int rawValues[NUM_SENSORS];

  for (int sample = 0; sample < samples; sample++)
  {
    sensor.readRaw(rawValues);

    Serial.print(F("  "));
    if (sample < 10)
      Serial.print(F(" "));
    Serial.print(sample + 1);
    Serial.print(F("   | "));

    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if (rawValues[i] < 100)
        Serial.print(F(" "));
      if (rawValues[i] < 10)
        Serial.print(F(" "));
      Serial.print(rawValues[i]);
      Serial.print(F("  "));

      sensor._whiteValues[i] += rawValues[i];
    }
    Serial.println();

    digitalWrite(LED_PIN, sample % 2);

    delay(delayMs);
  }

  Serial.println(F("\n--- Average WHITE values ---"));
  Serial.print(F("Values: "));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensor._whiteValues[i] /= samples;
    Serial.print(sensor._whiteValues[i]);
    Serial.print(F(" "));
  }
  Serial.println(F("\n✓ Calib trang hoan thanh!"));

  ledSteady();
}

void calibrateBlackWithLED()
{
  Serial.println(F("\n=== CALIB DEN ==="));
  Serial.println(F("Dat tren vach den..."));
  Serial.println(F("Bat dau sau 2 giay...\n"));

  ledBlinkSlow(2000);

  Serial.println(F("Dang calib... LED nhay nhanh"));

  const int samples = 50;
  const int delayMs = 10;

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensor._blackValues[i] = 0;
  }

  Serial.println(F("Reading 50 samples..."));
  Serial.println(F("Sample | S0    S1    S2    S3    S4"));
  Serial.println(F("-------|---------------------------"));

  int rawValues[NUM_SENSORS];

  for (int sample = 0; sample < samples; sample++)
  {
    sensor.readRaw(rawValues);

    Serial.print(F("  "));
    if (sample < 10)
      Serial.print(F(" "));
    Serial.print(sample + 1);
    Serial.print(F("   | "));

    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if (rawValues[i] < 100)
        Serial.print(F(" "));
      if (rawValues[i] < 10)
        Serial.print(F(" "));
      Serial.print(rawValues[i]);
      Serial.print(F("  "));

      sensor._blackValues[i] += rawValues[i];
    }
    Serial.println();

    digitalWrite(LED_PIN, sample % 2);

    delay(delayMs);
  }

  Serial.println(F("\n--- Average BLACK values ---"));
  Serial.print(F("Values: "));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensor._blackValues[i] /= samples;
    Serial.print(sensor._blackValues[i]);
    Serial.print(F(" "));
  }
  Serial.println(F("\n✓ Calib den hoan thanh!"));

  ledSteady();
}

// ========== BUTTON HANDLER ==========
void handleButton()
{
  static bool lastStableState = LOW;
  static unsigned long lastChangeTime = 0;

  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastStableState)
  {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > DEBOUNCE_DELAY)
  {
    if (reading == HIGH && lastButtonState == LOW)
    {
      buttonPressCount++;

      Serial.print(F("\n>>> BUTTON PRESSED: "));
      Serial.print(buttonPressCount);
      Serial.println(F(" <<<"));

      buzzer.beep(2000, 100);

      switch (currentState)
      {
      case STATE_IDLE:
        currentState = STATE_CALIB_WHITE;
        Serial.println(F("STATE: CALIB_WHITE"));
        Serial.println(F("Press button again to start white calibration..."));
        ledSteady();
        break;

      case STATE_CALIB_WHITE:
        calibrateWhiteWithLED();
        currentState = STATE_CALIB_BLACK;
        Serial.println(F("STATE: CALIB_BLACK"));
        Serial.println(F("Press button again to start black calibration..."));
        buzzer.success();
        break;

      case STATE_CALIB_BLACK:
        calibrateBlackWithLED();
        sensor.calculateThresholds();

        currentState = STATE_READY;
        Serial.println(F("STATE: READY"));
        Serial.println(F("✓ Calibration complete!"));
        Serial.println(F("📢 Press button again to START mission..."));
        buzzer.success();
        ledSteady();
        break;

      case STATE_READY:
        currentState = STATE_MISSION_RUNNING;
        Serial.println(F("STATE: MISSION_RUNNING"));
        Serial.println(F("Starting mission in 2 seconds..."));
        buzzer.startup();
        ledOff();
        delay(2000);
        break;

      case STATE_MISSION_RUNNING:
        Serial.println(F("⚠ Mission already running!"));
        buzzer.warning();
        break;
      }
    }

    lastButtonState = reading;
  }

  lastStableState = reading;
}

// ========== SETUP ==========
void setup()
{
  Serial.begin(115200);
  delay(500);

  Serial.println(F("=============================="));
  Serial.println(F("LINE FOLLOWER ROBOT"));
  Serial.println(F("5-Sensor with PID Control"));
  Serial.println(F("Mission Mode"));
  Serial.println(F("==============================\n"));

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  ledSteady();

  ArmInit();

  motor.begin();
  motor.setGain(0.90f, 0.95f);
  motor.setBias(0, 0);
  motor.setPWMCap(140);
  motor.setPolarity(false);

  sensor.begin();

  pid.begin();
  pid.setGains(1.0f, 0.02f, 3.0f);
  pid.setSpeed(70, 120);
  pid.setSetpoint(200);

  buzzer.begin();
  buzzer.startup();

  Serial.println(F("✓ Hardware initialized!"));
  Serial.println(F("Press button to start calibration...\n"));

  Serial.print(F("Setpoint: "));
  Serial.println(200);
}

// ========== MISSION FUNCTIONS ==========

void mission1()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("🚀 STARTING MISSION 1"));
  Serial.println(F("========================================\n"));

  // VÒNG WHILE 1: Dò line đến 0b11111
  Serial.println(F("\n--- TASK 1: Follow line until 0b11111 ---"));
  pid.setEnabled(true);
  motor.move(100, 120, 300);
  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Rẽ phải
  Serial.println(F("→ Turning RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 65, 350);
  delay(200);
  motor.move(140, -130, 350);
  motor.stop();
  delay(300);

  // VÒNG WHILE 2: Dò line đến 0b11100
  Serial.println(F("\n--- TASK 2: Follow line until 0b11100 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11100)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11100 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Đi thẳng
  Serial.println(F("→ Moving STRAIGHT"));
  pid.setEnabled(false);
  motor.move(70, 80, 100);

  // VÒNG WHILE 3: Dò line đến 0b11111 lần 2
  Serial.println(F("\n--- TASK 3: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected - FINAL!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Dừng 3 giây
  pid.setEnabled(false);
  Nhiemvu1();
  delay(3000);
  Serial.println(F("\n✅ MISSION 1 COMPLETE!\n"));
}

void mission2()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("🚀 STARTING MISSION 2"));
  Serial.println(F("========================================\n"));

  // Quay ngược xe lại
  Serial.println(F("→ Reversing robot"));
  pid.setEnabled(false);
  motor.move(-180, 200, 560); // quay ngược xe 180 độ
  motor.stop();
  delay(300);
  motor.move(-100, -120, 450); // lùi 1 chút
  motor.stop();
  delay(300);
  Reset_lentren();
  delay(300);
  // VÒNG WHILE 4: Dò line đến 0b00111
  Serial.println(F("\n--- TASK 4: Follow line until 0b00111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b00111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b00111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tiến và rẽ phải
  Serial.println(F("→ Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 60, 300);
  delay(200);
  motor.move(140, -130, 200);
  motor.stop();
  delay(300);

  // VÒNG WHILE 5: Dò line đến 0b11100
  Serial.println(F("\n--- TASK 5: Follow line until 0b11100 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11100)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11100 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Dừng, tiến, rẽ phải
  Serial.println(F("→ Stop, move forward, then turn RIGHT"));
  pid.setEnabled(false);
  motor.stop();
  delay(200);
  motor.move(40, 65, 450);
  motor.stop();
  delay(500);
  motor.move(140, -130, 450);

  motor.stop();
  delay(300);

  delay(2000); // chờ thêmn servor

  Serial.println(F("\n MISSION 2 COMPLETE!\n"));
  //=========================================
  //============= END OF MISSION 2 ==========
  //=========================================
}

void mission3()
{
  Serial.println(F("\n========================================"));
  Serial.println(F(" STARTING MISSION 3"));
  Serial.println(F("========================================\n"));

  // Lùi lại
  Serial.println(F("→ Reversing"));
  pid.setEnabled(false);
  motor.move(-30, -60, 350);
  motor.stop();
  delay(200);

  // Quay trái
  Serial.println(F("→ Turning LEFT"));
  motor.move(-130, 140, 320);
  motor.stop();
  delay(300);

  // VÒNG WHILE 6: Dò line đến 0b00111
  Serial.println(F("\n--- TASK 6: Follow line until 0b00111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b00111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b00111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }
    pid.update();
    delay(1);
  }

  // Tiến và rẽ phải
  Serial.println(F("→ Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(50, 70, 350);
  delay(200);

  motor.move(140, -135, 300);
  motor.stop();
  delay(300);

  // VÒNG WHILE 7: Dò line đến 0b11111
  Serial.println(F("\n--- TASK 7: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tiến và rẽ phải
  Serial.println(F("→ Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 60, 400);
  delay(200);
  motor.move(140, -135, 430);
  motor.stop();
  delay(300);

  // VÒNG WHILE 8: Dò line đến 0b11111 (final)
  Serial.println(F("\n--- TASK 8: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected - FINAL STOP!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Dừng và delay 3 giây
  Serial.println(F("→ STOPPING for 3000ms"));
  pid.setEnabled(false);
  motor.stop();
  Nhiemvu3();
  buzzer.success();
  delay(2000);

  Serial.println(F("\n✅ MISSION 3 COMPLETE!\n"));
  //=========================================
  //============= END OF MISSION 3 ==========
  //=========================================
}
void mission4()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("🚀 STARTING MISSION 4"));
  Serial.println(F("========================================\n"));

  // Quay 180 độ (tham khảo Mission 2)
  Serial.println(F("→ Rotating 180 degrees"));
  pid.setEnabled(false);
  motor.move(-230, 250, 720); // Quay 180° (giống Mission 2)
  motor.stop();
  delay(300);

  // VÒNG WHILE 9: Dò line đến 0b11111
  Serial.println(F("\n--- TASK 9: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break; // ✅ THOÁT VÒNG WHILE 9
      }
    }

    pid.update();
    delay(1);
  }

  // Tiến 1 chút rồi rẽ phải
  Serial.println(F("→ Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 65, 300); // Tiến 1 chút (tham khảo Mission 1)
  delay(200);
  motor.move(150, -130, 350); // Rẽ phải (tham khảo Mission 1)
  motor.stop();
  delay(300);

  // VÒNG WHILE 10: Dò line đến 0b11111 (final)
  Serial.println(F("\n--- TASK 10: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("✓ Junction 0b11111 detected - FINAL STOP!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break; // ✅ THOÁT VÒNG WHILE 10
      }
    }

    pid.update();
    delay(1);
  }

  // Dừng và delay 3 giây
  Serial.println(F("→ STOPPING for 3000ms"));
  pid.setEnabled(false);
  motor.stop();
  buzzer.success();
  Nhiemvu4();
  delay(2000);

  Serial.println(F("\n========================================"));
  Serial.println(F("✅ MISSION 4 COMPLETE!"));
  Serial.println(F("========================================\n"));

  //=========================================
  //============= END OF MISSION 4 ==========
  //=========================================
}

void mission5()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("STARTING MISSION 5"));
  Serial.println(F("========================================\n"));

  // Quay 180 do
  Serial.println(F("-> Rotating 180 degrees"));
  pid.setEnabled(false);
  motor.move(-40, -40, 800);
  delay(200);
  motor.stop();
  motor.move(-230, 250, 550); // Quay 180° (giống Mission 2)
  motor.stop();
  delay(300);

  // VONG WHILE 11: Do line den 0b11111
  Serial.println(F("\n--- TASK 11: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut roi re phai
  Serial.println(F("-> Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 65, 350);
  delay(200);
  motor.move(150, -130, 280);
  motor.stop();
  delay(300);

  // VONG WHILE 12: Do line den 0b11111 (final)
  Serial.println(F("\n--- TASK 12: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected - FINAL STOP!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Dung va delay 3 giay
  Serial.println(F("-> STOPPING for 3000ms"));
  pid.setEnabled(false);
  Nhiemvu5();
  motor.stop();
  buzzer.success();

  delay(2000);

  Serial.println(F("\n========================================"));
  Serial.println(F("MISSION 5 COMPLETE!"));
  Serial.println(F("========================================\n"));

  //=========================================
  //============= END OF MISSION 5 ==========
  //=========================================
}

void mission6()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("STARTING MISSION 6"));
  Serial.println(F("========================================\n"));

  // Quay 180 do
  Serial.println(F("-> Rotating 180 degrees"));
  pid.setEnabled(false);
  motor.move(-230, 250, 460); // Quay 180° (giống Mission 2)
  motor.stop();
  delay(300);
  Reset_lentren();
  delay(300);
  // VONG WHILE 13: Do line den 0b11111
  Serial.println(F("\n--- TASK 13: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut roi quay phai
  Serial.println(F("-> Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 65, 300);
  delay(200);
  motor.move(150, -130, 250);
  motor.stop();
  delay(300);

  // VONG WHILE 14: Do line den 0b11111
  Serial.println(F("\n--- TASK 14: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut roi re phai
  Serial.println(F("-> Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 60, 300);
  delay(200);
  motor.move(150, -130, 250);
  motor.stop();
  delay(300);

  // VONG WHILE 15: Do line den 0b11100
  Serial.println(F("\n--- TASK 15: Follow line until 0b11100 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11100)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11100 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 340ms roi quay trai
  Serial.println(F("-> Moving forward 340ms then turn LEFT"));
  pid.setEnabled(false);
  motor.move(50, 60, 330);
  delay(200);
  motor.move(-150, 140, 250); // Quay trai (nguoc voi quay phai)
  motor.stop();
  delay(300);

  // VONG WHILE 16: Do line den 0b11100
  Serial.println(F("\n--- TASK 16: Follow line until 0b11100 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11100)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11100 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 340ms roi quay phai
  Serial.println(F("-> Moving forward 340ms then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(40, 60, 350);
  delay(200);
  motor.move(140, -130, 200); // Quay phai
  motor.stop();
  Nhiemvu6();
  delay(2000); // Delay 3 giay

  Serial.println(F("\n========================================"));
  Serial.println(F("MISSION 6 COMPLETE!"));
  Serial.println(F("========================================\n"));
}

void mission7()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("STARTING MISSION 7"));
  Serial.println(F("========================================\n"));

  // VONG WHILE 17: Do line den 0b11111
  Serial.println(F("\n--- TASK 17: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut roi quay phai
  Serial.println(F("-> Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(80, 105, 350);
  delay(200);
  motor.move(150, -130, 310);
  motor.stop();
  delay(300);

  // VONG WHILE 18: Do line den 0b11111 (final)
  Serial.println(F("\n--- TASK 18: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected - FINAL STOP!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Dung lai va delay 3 giay
  Serial.println(F("-> STOPPING for 3000ms"));
  pid.setEnabled(false);
  motor.stop();
  buzzer.success();
  delay(2000);

  Serial.println(F("\n========================================"));
  Serial.println(F("MISSION 7 COMPLETE!"));
  Serial.println(F("========================================\n"));

  //=========================================
  //============= END OF MISSION 7 ==========
  //=========================================
}
void missionEnd()
{
  Serial.println(F("\n========================================"));
  Serial.println(F("STARTING MISSION END"));
  Serial.println(F("========================================\n"));

  // Quay 180 do
  Serial.println(F("-> Rotating 180 degrees"));
  pid.setEnabled(false);
  motor.move(-150, 150, 520);
  motor.stop();
  delay(300);

  // Lui 1 chut (giong mission 2)
  Serial.println(F("-> Reversing a bit"));
  motor.move(-120, -160, 300);
  motor.stop();
  delay(300);

  // VONG WHILE 19: Do line den 0b11100
  Serial.println(F("\n--- TASK 19: Follow line until 0b11100 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11100)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11100 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut
  Serial.println(F("-> Moving forward"));
  pid.setEnabled(false);
  motor.move(80, 100, 340);
  motor.stop();
  delay(200);

  // VONG WHILE 20: Do line den 0b11111
  Serial.println(F("\n--- TASK 20: Follow line until 0b11111 ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 1 chut va quay phai
  Serial.println(F("-> Moving forward then turn RIGHT"));
  pid.setEnabled(false);
  motor.move(80, 105, 350);
  delay(200);
  motor.move(150, -130, 310);
  motor.stop();
  delay(300);

  // VONG WHILE 21: Do line den 0b11111 (final)
  Serial.println(F("\n--- TASK 21: Follow line until 0b11111 (final) ---"));
  pid.setEnabled(true);

  while (true)
  {
    buzzer.update();

    byte sensors = sensor.getDigitalByte();

    if (sensors == 0b11111)
    {
      if (millis() - lastJunctionTime > JUNCTION_COOLDOWN)
      {
        Serial.println(F("Junction 0b11111 detected - FINAL STOP!"));
        buzzer.beep(2500, 200);
        lastJunctionTime = millis();
        break;
      }
    }

    pid.update();
    delay(1);
  }

  // Tien 500ms roi dung
  Serial.println(F("-> Moving forward 500ms then STOP"));
  pid.setEnabled(false);
  motor.move(90, 110, 500);
  motor.stop();
  delay(2000); // Delay 2 giay

  Serial.println(F("\n========================================"));
  Serial.println(F("MISSION END COMPLETE!"));
  Serial.println(F("========================================\n"));

  //=========================================
  //============= END OF MISSION END ========
  //=========================================
}

// ========== LOOP ==========
void loop()
{
  buzzer.update();
  handleButton();

  // Heartbeat khi chưa chạy mission
  if (currentState != STATE_MISSION_RUNNING)
  {
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 2000)
    {
      Serial.print(F("Waiting... State: "));
      Serial.print(currentState);
      Serial.print(F(" | Button: "));
      Serial.println(buttonPressCount);
      lastHeartbeat = millis();
    }

    delay(1);
    return;
  }

  // ========== BẮT ĐẦU MISSIONS ==========
  Serial.println(F("\n========================================"));
  Serial.println(F("🚀 STARTING ALL MISSIONS"));
  Serial.println(F("========================================\n"));
  // mission1();
  // mission2();
  // mission3();
  // mission4();
  mission5();
  mission6();
  mission7();
  missionEnd();
  // Thêm mission 4, 5, ... ở đây

  // ========== MISSION COMPLETE ==========
  Serial.println(F("\n========================================"));
  Serial.println(F("✅ ALL MISSIONS COMPLETE!"));
  Serial.println(F("========================================\n"));

  buzzer.startup();
  ledSteady();

  // Vòng while rỗng
  Serial.println(F("Entering infinite loop (mission done)...\n"));

  while (true)
  {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 200)
    {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastBlink = millis();
    }

    buzzer.update();
    delay(10);
  }
}