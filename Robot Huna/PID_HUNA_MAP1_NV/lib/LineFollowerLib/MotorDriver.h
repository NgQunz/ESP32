#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>
#include <SoftPWM.h>

/**
 * @brief Unified Motor Driver
 * 
 * Tích hợp:
 * - MotorDriver (line following)
 * - Motor_control_basic (distance-based control)
 */
class MotorDriver {
public:
  MotorDriver(uint8_t leftDir, uint8_t leftPWM, 
              uint8_t rightDir, uint8_t rightPWM);
  
  void begin();
  
  // ========== CORE API (MotorDriver) ==========
  void setSpeed(int leftSpeed, int rightSpeed);
  void move(int leftSpeed, int rightSpeed, unsigned long duration = 0);
  void stop();
  
  // Configuration
  void setGain(float leftGain, float rightGain);
  void setBias(int leftBias, int rightBias);
  void setPWMCap(int maxPWM);
  void setPolarity(bool forwardIsHigh);
  
  // ========== LEGACY API (Motor_control_basic) ==========
  
  /**
   * @brief Set target speeds (legacy, giống Speed() cũ)
   * @param left Left motor speed (-255 to 255)
   * @param right Right motor speed (-255 to 255)
   * 
   * Chỉ set tốc độ, KHÔNG chạy ngay.
   * Cần gọi Distance() sau đó.
   */
  void Speed(int left, int right);
  
  /**
   * @brief Move by distance (legacy)
   * @param cm Distance in centimeters
   * 
   * Sử dụng tốc độ đã set bằng Speed().
   * Blocking function với active brake.
   */
  void Distance(float cm);
  
  /**
   * @brief Turn left (legacy)
   * @param degree Extra distance offset (NOT actual degrees)
   */
  void turn_Left(int degree = 0);
  
  /**
   * @brief Turn right (legacy)
   * @param degree Extra distance offset (NOT actual degrees)
   */
  void turn_Right(int degree = 0);
  
  /**
   * @brief Run with distance sensor compensation (legacy)
   * @param distance Distance in cm (will be scaled by 1.2x)
   */
  void runWithDS(int distance);

  uint8_t _leftDir, _leftPWM;
  uint8_t _rightDir, _rightPWM;
  
  float _gainLeft, _gainRight;
  int _biasLeft, _biasRight;
  int _pwmCap;
  bool _forwardIsHigh;
  
  // Legacy parameters (from Motor_control_basic)
  int _targetLeft, _targetRight;
  float _cmPerSecBase;
  int _speedCalPWM;
  float _cmScale;
  float _cmOffset;
  float _nearOffsetCm;
  float _nearApplyUpto;
  int _brakeBackPWM;
  uint16_t _brakeDurationMs;
  
  void applyMotor(int speed, uint8_t dirPin, uint8_t pwmPin);
  int applyTrim(int speed, float gain, int bias);
  inline uint8_t directionForward() { return _forwardIsHigh ? HIGH : LOW; }
  inline uint8_t directionReverse() { return _forwardIsHigh ? LOW : HIGH; }
  
  // Legacy helper functions
  void applyTrimAndWrite(int leftSigned, int rightSigned);
  void stopActiveBrake();
  unsigned long msForDistance_linear(float cm_req, int pwmL, int pwmR);
};

#endif
