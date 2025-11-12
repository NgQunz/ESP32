#include <Arduino.h>
#include <SoftPWM.h>

/* ================== PINOUT ================== */
#define ML_DIR 1
#define MR_DIR 0
#define ML_PWM 8
#define MR_PWM 6

#define BUTTON_PIN 15

#define SENSOR_0 A0
#define SENSOR_1 A1
#define SENSOR_2 A9
#define SENSOR_3 A2
#define SENSOR_4 A3

/* ================== CONFIG ================== */
#define NUM_SENSORS 5
#define CALIBRATION_SAMPLES 50
#define CALIBRATION_DELAY 10
#define DEBOUNCE_DELAY 50

const bool FORWARD_IS_HIGH = false;
inline uint8_t DIR_FWD() { return FORWARD_IS_HIGH ? HIGH : LOW; }
inline uint8_t DIR_REV() { return FORWARD_IS_HIGH ? LOW : HIGH; }

const int PWM_CAP = 140;
inline int capPWM(int v) { return constrain(v, 0, PWM_CAP); }

float GAIN_L = 0.90f, GAIN_R = 0.95f;
int BIAS_L = 0, BIAS_R = 0;

/* ================== PID CONFIG ================== */
#define DEFAULT_KP 0.8
#define DEFAULT_KI 0.01
#define DEFAULT_KD 2.2

#define BASE_SPEED 55
#define MAX_SPEED 100

/* ================== GLOBAL VARIABLES ================== */
int sensorPins[NUM_SENSORS] = {A0, A1, A9, A2, A3};
int sensorValues[NUM_SENSORS];

// ĐỔI SANG LONG ĐỂ TRÁNH OVERFLOW
long whiteValues[NUM_SENSORS]; // ← SỬA ĐÂY
long blackValues[NUM_SENSORS]; // ← SỬA ĐÂY

int thresholdValues[NUM_SENSORS];
bool digitalSensors[NUM_SENSORS];

// PID
float kp = DEFAULT_KP;
float ki = DEFAULT_KI;
float kd = DEFAULT_KD;
int setpoint = (NUM_SENSORS - 1) * 100 / 2;

int lastError = 0;
float integral = 0;
unsigned long lastPIDTime = 0;

// State machine
enum State
{
  STATE_IDLE,
  STATE_CALIB_WHITE,
  STATE_CALIB_BLACK,
  STATE_RUNNING
};

State currentState = STATE_IDLE;
int buttonPressCount = 0;

bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;

bool isCalibrated = false;
bool robotRunning = false;
int linePosition = 0;
int lastValidPosition = setpoint;

/* ================== MOTOR CONTROL ================== */
void setWheelSigned(int pwmSigned, uint8_t dirPin, uint8_t pwmCh)
{
  pwmSigned = constrain(pwmSigned, -255, 255);
  if (pwmSigned > 0)
  {
    digitalWrite(dirPin, DIR_FWD());
    SoftPWMSet(pwmCh, capPWM(pwmSigned));
  }
  else if (pwmSigned < 0)
  {
    digitalWrite(dirPin, DIR_REV());
    SoftPWMSet(pwmCh, capPWM(-pwmSigned));
  }
  else
  {
    SoftPWMSet(pwmCh, 0);
  }
}

void applyTrimAndWrite(int leftSigned, int rightSigned)
{
  int sL = (leftSigned >= 0) ? +1 : -1;
  int sR = (rightSigned >= 0) ? +1 : -1;
  int mL = abs(leftSigned);
  int mR = abs(rightSigned);

  long l = lround(mL * GAIN_L) + BIAS_L;
  long r = lround(mR * GAIN_R) + BIAS_R;

  setWheelSigned(sL * capPWM((int)l), ML_DIR, ML_PWM);
  setWheelSigned(sR * capPWM((int)r), MR_DIR, MR_PWM);
}

void stopMotors()
{
  SoftPWMSet(ML_PWM, 0);
  SoftPWMSet(MR_PWM, 0);
}

/* ================== SENSOR READING ================== */
void readAllSensors()
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensorValues[i] = analogRead(sensorPins[i]);
    delay(1);
  }
}

/* ================== CALIBRATION ================== */
void calibrateWhite()
{
  Serial.println(F("\n=== CALIB TRANG ==="));
  Serial.println(F("Dat tren nen trang..."));
  Serial.println(F("Bat dau sau 2 giay...\n"));
  delay(2000);

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    whiteValues[i] = 0;
  }

  Serial.println(F("Dang doc 50 lan (moi lan cach 10ms)..."));
  Serial.println(F("Sample | S0    S1    S2    S3    S4"));
  Serial.println(F("-------|---------------------------"));

  for (int sample = 0; sample < CALIBRATION_SAMPLES; sample++)
  {
    readAllSensors();

    Serial.print(F("  "));
    if (sample < 10)
      Serial.print(F(" "));
    Serial.print(sample + 1);
    Serial.print(F("   | "));

    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if (sensorValues[i] < 100)
        Serial.print(F(" "));
      if (sensorValues[i] < 10)
        Serial.print(F(" "));
      Serial.print(sensorValues[i]);
      Serial.print(F("  "));

      whiteValues[i] += sensorValues[i];
    }
    Serial.println();

    delay(CALIBRATION_DELAY);
  }

  Serial.println(F("\n--- Ket qua trung binh ---"));
  Serial.print(F("White values: "));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    whiteValues[i] /= CALIBRATION_SAMPLES;
    Serial.print(whiteValues[i]);
    Serial.print(F(" "));
  }
  Serial.println(F("\n✓ Calib trang hoan thanh!"));
}

void calibrateBlack()
{
  Serial.println(F("\n=== CALIB DEN ==="));
  Serial.println(F("Dat tren vach den..."));
  Serial.println(F("Bat dau sau 2 giay...\n"));
  delay(2000);

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    blackValues[i] = 0;
  }

  Serial.println(F("Dang doc 50 lan (moi lan cach 10ms)..."));
  Serial.println(F("Sample | S0    S1    S2    S3    S4"));
  Serial.println(F("-------|---------------------------"));

  for (int sample = 0; sample < CALIBRATION_SAMPLES; sample++)
  {
    readAllSensors();

    Serial.print(F("  "));
    if (sample < 10)
      Serial.print(F(" "));
    Serial.print(sample + 1);
    Serial.print(F("   | "));

    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if (sensorValues[i] < 100)
        Serial.print(F(" "));
      if (sensorValues[i] < 10)
        Serial.print(F(" "));
      Serial.print(sensorValues[i]);
      Serial.print(F("  "));

      blackValues[i] += sensorValues[i];
    }
    Serial.println();

    delay(CALIBRATION_DELAY);
  }

  Serial.println(F("\n--- Ket qua trung binh ---"));
  Serial.print(F("Black values: "));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    blackValues[i] /= CALIBRATION_SAMPLES;
    Serial.print(blackValues[i]);
    Serial.print(F(" "));
  }
  Serial.println(F("\n✓ Calib den hoan thanh!"));
}

void calculateThresholds()
{
  Serial.println(F("\n=== TINH THRESHOLD ==="));

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    thresholdValues[i] = (whiteValues[i] + blackValues[i]) / 2;
  }
  isCalibrated = true;

  Serial.println(F("\nSensor | White | Black | Threshold"));
  Serial.println(F("-------|-------|-------|----------"));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(F("  S"));
    Serial.print(i);
    Serial.print(F("   | "));
    Serial.print(whiteValues[i]);
    if (whiteValues[i] < 100)
      Serial.print(F(" "));
    if (whiteValues[i] < 10)
      Serial.print(F(" "));
    Serial.print(F("  | "));
    Serial.print(blackValues[i]);
    if (blackValues[i] < 100)
      Serial.print(F(" "));
    if (blackValues[i] < 10)
      Serial.print(F(" "));
    Serial.print(F("  | "));
    Serial.println(thresholdValues[i]);
  }

  Serial.println(F("\n✓ Tinh threshold hoan thanh!"));
  Serial.println(F("San sang bat dau do line!\n"));
}

/* ================== SENSOR PROCESSING ================== */
void digitizeSensors()
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    digitalSensors[i] = (sensorValues[i] < thresholdValues[i]);
  }
}

int calculateLinePosition()
{
  if (!isCalibrated)
    return setpoint;

  digitizeSensors();

  bool lineDetected = false;
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if (digitalSensors[i])
    {
      lineDetected = true;
      break;
    }
  }

  if (!lineDetected)
  {
    return lastValidPosition;
  }

  long weightedSum = 0;
  long sumSensors = 0;

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if (digitalSensors[i])
    {
      weightedSum += (long)i * 100;
      sumSensors++;
    }
  }

  if (sumSensors > 0)
  {
    int position = weightedSum / sumSensors;
    lastValidPosition = position;
    return position;
  }

  return lastValidPosition;
}

/* ================== PID CONTROL ================== */
void pidControl()
{
  if (!isCalibrated)
  {
    stopMotors();
    return;
  }

  readAllSensors();
  linePosition = calculateLinePosition();

  int error = linePosition - setpoint;

  unsigned long currentTime = millis();
  float deltaTime = 0.02;
  if (lastPIDTime > 0)
  {
    deltaTime = (currentTime - lastPIDTime) / 1000.0;
  }
  lastPIDTime = currentTime;
  deltaTime = constrain(deltaTime, 0.005, 0.1);

  float proportional = error;
  integral += error * deltaTime;
  integral = constrain(integral, -500, 500);

  float derivative = (error - lastError) / deltaTime;
  lastError = error;

  float pidOutput = (kp * proportional) + (ki * integral) + (kd * derivative);
  int correction = constrain(pidOutput, -MAX_SPEED, MAX_SPEED);

  int leftSpeed = BASE_SPEED - correction;
  int rightSpeed = BASE_SPEED + correction;

  leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);

  applyTrimAndWrite(leftSpeed, rightSpeed);
}

/* ================== BUTTON HANDLER ================== */
void handleButton()
{
  static bool lastStableState = LOW;
  static unsigned long lastChangeTime = 0;

  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastStableState)
  {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > 50)
  {

    if (reading == HIGH && lastButtonState == LOW)
    {
      buttonPressCount++;

      Serial.print(F("\n>>> BUTTON PRESSED: "));
      Serial.print(buttonPressCount);
      Serial.println(F(" <<<"));

      switch (currentState)
      {
      case STATE_IDLE:
        currentState = STATE_CALIB_WHITE;
        Serial.println(F("STATE: CALIB_WHITE"));
        Serial.println(F("Nhan nut lan nua de bat dau calib trang..."));
        break;

      case STATE_CALIB_WHITE:
        calibrateWhite();
        currentState = STATE_CALIB_BLACK;
        Serial.println(F("STATE: CALIB_BLACK"));
        Serial.println(F("Nhan nut lan nua de bat dau calib den..."));
        break;

      case STATE_CALIB_BLACK:
        calibrateBlack();
        calculateThresholds();
        currentState = STATE_RUNNING;
        robotRunning = true;
        Serial.println(F("STATE: RUNNING"));
        break;

      case STATE_RUNNING:
        robotRunning = false;
        stopMotors();
        currentState = STATE_IDLE;
        buttonPressCount = 0;
        Serial.println(F("STATE: IDLE (STOPPED)"));
        Serial.println(F("Nhan nut de bat dau lai..."));
        break;
      }
    }

    lastButtonState = reading;
  }

  lastStableState = reading;
}

/* ================== PRINT SENSOR STATUS ================== */
void printSensorStatus()
{
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 200)
    return;
  lastPrint = millis();

  Serial.print(F("BTN: "));
  Serial.print(buttonPressCount);
  Serial.print(F(" | Sensors: "));

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(digitalSensors[i] ? '1' : '0');
    if (i < NUM_SENSORS - 1)
      Serial.print(F(" "));
  }

  Serial.print(F(" | Pos: "));
  Serial.print(linePosition);
  Serial.print(F(" | Err: "));
  Serial.println(linePosition - setpoint);
}

/* ================== SETUP ================== */
void setup()
{
  delay(1500);

  Serial.begin(115200);

  delay(500);

  Serial.println(F("=============================="));
  Serial.println(F("LEONARDO SERIAL WORKING!"));
  Serial.println(F("=============================="));

  Serial.println(F("\n=== 5-SENSOR LINE FOLLOWER ==="));

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(ML_DIR, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_PWM, OUTPUT);

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    pinMode(sensorPins[i], INPUT);
  }

  SoftPWMBegin();
  SoftPWMSet(ML_PWM, 0);
  SoftPWMSet(MR_PWM, 0);

  Serial.println(F("✓ Hardware initialized!"));
  Serial.println(F("Press button to start calibration..."));
  Serial.print(F("Setpoint: "));
  Serial.println(setpoint);

  Serial.print(F("Button initial state: "));
  Serial.println(digitalRead(BUTTON_PIN) ? F("HIGH") : F("LOW"));
}

/* ================== LOOP ================== */
void loop()
{
  handleButton();

  if (robotRunning && isCalibrated)
  {
    pidControl();
    printSensorStatus();
  }
  else
  {
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 2000)
    {
      Serial.print(F("Waiting... BTN: "));
      Serial.print(buttonPressCount);
      Serial.print(F(" | Button state: "));
      Serial.println(digitalRead(BUTTON_PIN) ? F("HIGH") : F("LOW"));
      lastHeartbeat = millis();
    }
  }

  delay(1);
}