#include "MotorDriver.h"

MotorDriver::MotorDriver(uint8_t leftDir, uint8_t leftPWM,
                         uint8_t rightDir, uint8_t rightPWM)
    : _leftDir(leftDir), _leftPWM(leftPWM),
      _rightDir(rightDir), _rightPWM(rightPWM),
      _gainLeft(1.0f), _gainRight(1.0f),
      _biasLeft(0), _biasRight(0),
      _pwmCap(255), _forwardIsHigh(false),
      // Legacy parameters initialization
      _targetLeft(0), _targetRight(0),
      _cmPerSecBase(34.5f), _speedCalPWM(70),
      _cmScale(0.909f), _cmOffset(4.545f),
      _nearOffsetCm(0.0f), _nearApplyUpto(20.0f),
      _brakeBackPWM(60), _brakeDurationMs(120)
{
}

void MotorDriver::begin()
{
  pinMode(_leftDir, OUTPUT);
  pinMode(_rightDir, OUTPUT);
  pinMode(_leftPWM, OUTPUT);
  pinMode(_rightPWM, OUTPUT);

  SoftPWMBegin();
  stop();
}

// ========== CORE API ==========

void MotorDriver::setSpeed(int leftSpeed, int rightSpeed)
{
  // Apply trim
  int leftTrimmed = applyTrim(leftSpeed, _gainLeft, _biasLeft);
  int rightTrimmed = applyTrim(rightSpeed, _gainRight, _biasRight);

  // Apply to motors
  applyMotor(leftTrimmed, _leftDir, _leftPWM);
  applyMotor(rightTrimmed, _rightDir, _rightPWM);
}

void MotorDriver::move(int leftSpeed, int rightSpeed, unsigned long duration)
{
  // int speed = 30;
  // if (leftSpeed > 0)
  // {
  //   leftSpeed = leftSpeed - speed; // dương → giảm
  // }
  // else
  // {
  //   leftSpeed = leftSpeed + speed; // âm → tăng
  // }

  // if (rightSpeed > 0)
  // {
  //   rightSpeed = rightSpeed - speed; // dương → giảm
  // }
  // else
  // {
  //   rightSpeed = rightSpeed + speed; // âm → tăng
  // }

  setSpeed(rightSpeed, leftSpeed);

  if (duration > 0)
  {
    delay(duration);
    stop();
  }
}

void MotorDriver::stop()
{
  SoftPWMSet(_leftPWM, 0);
  SoftPWMSet(_rightPWM, 0);
}

void MotorDriver::setGain(float leftGain, float rightGain)
{
  _gainLeft = constrain(leftGain, 0.0f, 2.0f);
  _gainRight = constrain(rightGain, 0.0f, 2.0f);
}

void MotorDriver::setBias(int leftBias, int rightBias)
{
  _biasLeft = constrain(leftBias, -50, 50);
  _biasRight = constrain(rightBias, -50, 50);
}

void MotorDriver::setPWMCap(int maxPWM)
{
  _pwmCap = constrain(maxPWM, 0, 255);
}

void MotorDriver::setPolarity(bool forwardIsHigh)
{
  _forwardIsHigh = forwardIsHigh;
}

// ========== LEGACY API (Motor_control_basic compatibility) ==========

void MotorDriver::Speed(int left, int right)
{
  _targetLeft = constrain(left, -255, 255);
  _targetRight = constrain(right, -255, 255);
}

void MotorDriver::Distance(float cm)
{
  if (cm <= 0 || (_targetLeft == 0 && _targetRight == 0))
  {
    return;
  }

  // Apply speeds
  applyTrimAndWrite(_targetLeft, _targetRight);

  // Calculate hold time
  unsigned long hold_us = msForDistance_linear(cm, _targetLeft, _targetRight) * 1000UL;
  unsigned long t0 = micros();

  // Wait (blocking)
  while ((unsigned long)(micros() - t0) < hold_us)
  {
    // Busy wait
  }

  // Active brake
  stopActiveBrake();
}

void MotorDriver::turn_Left(int degree)
{
  Speed(-73, 70);
  Distance(32 + degree);
  Speed(0, 0);
}

void MotorDriver::turn_Right(int degree)
{
  Speed(70, -73);
  Distance(35 + degree);
  Speed(0, 0);
}

void MotorDriver::runWithDS(int distance)
{
  Speed(73, 73);
  Distance(distance * 1.2);
  Speed(0, 0);
}

// ========== PRIVATE METHODS ==========

void MotorDriver::applyMotor(int speed, uint8_t dirPin, uint8_t pwmPin)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(dirPin, directionForward());
    SoftPWMSet(pwmPin, min(speed, _pwmCap));
  }
  else if (speed < 0)
  {
    digitalWrite(dirPin, directionReverse());
    SoftPWMSet(pwmPin, min(-speed, _pwmCap));
  }
  else
  {
    SoftPWMSet(pwmPin, 0);
  }
}

int MotorDriver::applyTrim(int speed, float gain, int bias)
{
  int sign = (speed >= 0) ? 1 : -1;
  int magnitude = abs(speed);

  long result = lround(magnitude * gain) + bias;
  return sign * (int)result;
}

// ========== LEGACY HELPER METHODS ==========

void MotorDriver::applyTrimAndWrite(int leftSigned, int rightSigned)
{
  int sL = (leftSigned >= 0) ? +1 : -1;
  int sR = (rightSigned >= 0) ? +1 : -1;
  int mL = abs(leftSigned);
  int mR = abs(rightSigned);

  long l = lround(mL * _gainLeft) + _biasLeft;
  long r = lround(mR * _gainRight) + _biasRight;

  // Apply with PWM cap
  applyMotor(sL * min((int)l, _pwmCap), _leftDir, _leftPWM);
  applyMotor(sR * min((int)r, _pwmCap), _rightDir, _rightPWM);
}

void MotorDriver::stopActiveBrake()
{
  applyTrimAndWrite(-_brakeBackPWM, -_brakeBackPWM);
  delay(_brakeDurationMs);
  SoftPWMSet(_leftPWM, 0);
  SoftPWMSet(_rightPWM, 0);
}

unsigned long MotorDriver::msForDistance_linear(float cm_req, int pwmL, int pwmR)
{
  // Apply linear compensation
  float cm_eff = _cmScale * cm_req + _cmOffset;

  // Near distance compensation
  if (cm_req <= _nearApplyUpto)
  {
    cm_eff += _nearOffsetCm;
  }

  // Calculate velocity based on PWM
  int pwmMag = (abs(pwmL) + abs(pwmR)) / 2;
  float v_cm_s = _cmPerSecBase * ((float)pwmMag / (float)_speedCalPWM);

  // Calculate time
  float t_ms = (cm_eff / v_cm_s) * 1000.0f;

  return (unsigned long)(t_ms + 0.5f);
}
