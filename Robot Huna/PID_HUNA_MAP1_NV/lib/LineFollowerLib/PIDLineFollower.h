#ifndef PID_LINE_FOLLOWER_H
#define PID_LINE_FOLLOWER_H

#include <Arduino.h>
#include "MotorDriver.h"
#include "IRSensorArray.h"

/**
 * @brief PID controller for line following robot
 * 
 * Combines IRSensorArray and MotorDriver to follow a line using PID control.
 * 
 * Features:
 * - Classic PID controller (Proportional + Integral + Derivative)
 * - Anti-windup for integral term
 * - Configurable gains (Kp, Ki, Kd)
 * - Configurable speed parameters
 * - Debug methods for tuning
 */
class PIDLineFollower {
public:
  /**
   * @brief Constructor
   * @param motor Reference to MotorDriver object
   * @param sensor Reference to IRSensorArray object
   */
  PIDLineFollower(MotorDriver& motor, IRSensorArray& sensor);
  
  /**
   * @brief Initialize PID controller
   */
  void begin();
  
  /**
   * @brief Set PID gains
   * @param kp Proportional gain
   * @param ki Integral gain
   * @param kd Derivative gain
   */
  void setGains(float kp, float ki, float kd);
  
  /**
   * @brief Set speed parameters
   * @param baseSpeed Base speed for straight movement (0-255)
   * @param maxSpeed Maximum allowed speed (0-255)
   */
  void setSpeed(int baseSpeed, int maxSpeed);
  
  /**
   * @brief Set target line position (setpoint)
   * @param setpoint Target position (default is center = (numSensors-1)*100/2)
   */
  void setSetpoint(int setpoint);
  
  /**
   * @brief Reset PID controller (clear integral and derivative terms)
   */
  void reset();
  
  /**
   * @brief Update PID controller (call this continuously in loop)
   * 
   * This method:
   * 1. Reads sensor position
   * 2. Calculates PID error
   * 3. Computes correction
   * 4. Applies motor speeds
   */
  void update();
  
  /**
   * @brief Enable/disable PID controller
   * @param enabled true to enable, false to disable and stop motors
   */
  void setEnabled(bool enabled);
  
  /**
   * @brief Check if PID is enabled
   * @return true if enabled
   */
  bool isEnabled() const { return _enabled; }
  
  // ========== DEBUG GETTERS ==========
  
  /**
   * @brief Get current error
   * @return Error value (position - setpoint)
   */
  int getError() const { return _lastError; }
  
  /**
   * @brief Get current correction value
   * @return PID output correction
   */
  int getCorrection() const { return _lastCorrection; }
  
  /**
   * @brief Get current line position
   * @return Line position from sensor array
   */
  int getLinePosition() const { return _linePosition; }
  
  /**
   * @brief Get proportional term
   * @return P component of PID
   */
  float getProportional() const { return _proportional; }
  
  /**
   * @brief Get integral term
   * @return I component of PID
   */
  float getIntegral() const { return _integral; }
  
  /**
   * @brief Get derivative term
   * @return D component of PID
   */
  float getDerivative() const { return _derivative; }

  // ========== PUBLIC VARIABLES ==========
  
  MotorDriver& _motor;
  IRSensorArray& _sensor;
  
  // PID parameters
  float _kp, _ki, _kd;
  int _setpoint;
  int _baseSpeed;
  int _maxSpeed;
  
  // PID state variables
  float _proportional;
  float _integral;
  float _derivative;
  int _lastError;
  int _lastCorrection;
  int _linePosition;
  unsigned long _lastUpdateTime;
  
  // Control flags
  bool _enabled;
  

  static constexpr float INTEGRAL_MAX = 500.0f;  // Anti-windup limit
};

#endif
