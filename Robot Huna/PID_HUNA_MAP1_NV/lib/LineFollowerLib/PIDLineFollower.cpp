#include "PIDLineFollower.h"


PIDLineFollower::PIDLineFollower(MotorDriver& motor, IRSensorArray& sensor)
  : _motor(motor), _sensor(sensor),
    _kp(1.0f), _ki(0.02f), _kd(3.0f),
    _setpoint(200), // Default center position for 5 sensors
    _baseSpeed(70), _maxSpeed(120),
    _proportional(0), _integral(0), _derivative(0),
    _lastError(0), _lastCorrection(0), _linePosition(0),
    _lastUpdateTime(0), _enabled(false)
{
}

void PIDLineFollower::begin() {
  reset();
  _lastUpdateTime = millis();
}

void PIDLineFollower::setGains(float kp, float ki, float kd) {
  _kp = kp;
  _ki = ki;
  _kd = kd;
  
  Serial.print(F("PID Gains updated: Kp="));
  Serial.print(_kp, 3);
  Serial.print(F(" Ki="));
  Serial.print(_ki, 3);
  Serial.print(F(" Kd="));
  Serial.println(_kd, 3);
}

void PIDLineFollower::setSpeed(int baseSpeed, int maxSpeed) {
  _baseSpeed = constrain(baseSpeed, 0, 255);
  _maxSpeed = constrain(maxSpeed, 0, 255);
  
  Serial.print(F("Speed updated: Base="));
  Serial.print(_baseSpeed);
  Serial.print(F(" Max="));
  Serial.println(_maxSpeed);
}

void PIDLineFollower::setSetpoint(int setpoint) {
  _setpoint = setpoint;
  
  Serial.print(F("Setpoint updated: "));
  Serial.println(_setpoint);
}

void PIDLineFollower::reset() {
  _integral = 0;
  _derivative = 0;
  _lastError = 0;
  _proportional = 0;
  _lastCorrection = 0;
  
  Serial.println(F("PID reset"));
}

void PIDLineFollower::setEnabled(bool enabled) {
  _enabled = enabled;
  
  if (!enabled) {
    _motor.stop();
    reset();
  }
  
  Serial.print(F("PID "));
  Serial.println(enabled ? F("ENABLED") : F("DISABLED"));
}

void PIDLineFollower::update() {
  if (!_enabled) {
    _motor.stop();
    return;
  }
  
  // Check if sensor is calibrated
  if (!_sensor._calibrated) {  // ✅ Có thể truy cập trực tiếp vì public
    Serial.println(F("ERROR: Sensors not calibrated!"));
    _motor.stop();
    return;
  }
  
  // Calculate delta time
  unsigned long currentTime = millis();
  float deltaTime = 0.02f; // Default 20ms
  
  if (_lastUpdateTime > 0) {
    deltaTime = (currentTime - _lastUpdateTime) / 1000.0f;
  }
  _lastUpdateTime = currentTime;
  deltaTime = constrain(deltaTime, 0.005f, 0.1f); // Limit to 5-100ms
  
  // Read line position
  _linePosition = _sensor.getPosition();
  
  // Calculate error
  int error = _linePosition - _setpoint;
  
  // ========== PROPORTIONAL ==========
  _proportional = error;
  
  // ========== INTEGRAL ==========
  _integral += error * deltaTime;
  
  // Anti-windup: limit integral term
  _integral = constrain(_integral, -INTEGRAL_MAX, INTEGRAL_MAX);
  
  // ========== DERIVATIVE ==========
  _derivative = (error - _lastError) / deltaTime;
  
  _lastError = error;
  
  // ========== PID OUTPUT ==========
  float pidOutput = (_kp * _proportional) + 
                    (_ki * _integral) + 
                    (_kd * _derivative);
  
  int correction = constrain((int)pidOutput, -_maxSpeed, _maxSpeed);
  _lastCorrection = correction;
  
  // ========== MOTOR CONTROL ==========
  int leftSpeed = _baseSpeed - correction;
  int rightSpeed = _baseSpeed + correction;
  
  leftSpeed = constrain(leftSpeed, -_maxSpeed, _maxSpeed);
  rightSpeed = constrain(rightSpeed, -_maxSpeed, _maxSpeed);
  
  _motor.setSpeed(leftSpeed, rightSpeed);
}