#ifndef IR_SENSOR_ARRAY_H
#define IR_SENSOR_ARRAY_H

#include <Arduino.h>

/**
 * @brief Line pattern detection results
 */
enum LinePattern {
  PATTERN_STRAIGHT,       // Normal line following (1-2 sensors active)
  PATTERN_LEFT_TURN,      // Sharp left turn (leftmost sensors active)
  PATTERN_RIGHT_TURN,     // Sharp right turn (rightmost sensors active)
  PATTERN_T_JUNCTION,     // T-junction (3-4 sensors active)
  PATTERN_CROSS_JUNCTION, // Cross/4-way junction (4-5 sensors active)
  PATTERN_LOST,           // Line lost (no sensors active)
  PATTERN_END_LINE        // End line / horizontal bar (all sensors active)
};

/**
 * @brief IR sensor array for line following with calibration and pattern detection
 * 
 * Features:
 * - Read 5 analog IR sensors
 * - White/Black calibration with auto-threshold calculation
 * - Line position calculation (weighted average)
 * - Junction detection (T-junction, Cross, End line)
 * - Digital sensor output based on thresholds
 */
class IRSensorArray {
public:
  /**
   * @brief Constructor
   * @param pins Array of analog pin numbers
   * @param numSensors Number of sensors (typically 5)
   */
  IRSensorArray(const uint8_t* pins, uint8_t numSensors);
  
  /**
   * @brief Initialize sensor pins
   */
  void begin();

  // ========== SENSOR READING ==========


  /**
   * @brief Get digital sensor values as a byte
   * @return Byte with 5 bits (0b00000 to 0b11111)
   *         Bit 4 (MSB) = Sensor 0 (Leftmost)
   *         Bit 3       = Sensor 1
   *         Bit 2       = Sensor 2 (Center)
   *         Bit 1       = Sensor 3
   *         Bit 0 (LSB) = Sensor 4 (Rightmost)
   */
  byte getDigitalByte();  // ← THÊM HÀM MỚI
  
  // ========== SENSOR READING ==========
  
  /**
   * @brief Read raw analog values from all sensors
   * @param output Array to store raw values (0-1023)
   */
  void readRaw(int* output);
  
  /**
   * @brief Read digital values (0/1) based on thresholds
   * @param output Array to store digital values (true = black line detected)
   */
  void readDigital(bool* output);
  
  /**
   * @brief Get line position
   * @return Position value (0 to (numSensors-1)*100)
   *         Example for 5 sensors: 0 (far left) to 400 (far right), 200 = center
   */
  int getPosition();
  
  // ========== CALIBRATION ==========
  
  /**
   * @brief Calibrate white background
   * @param samples Number of samples to take (default 50)
   * @param delayMs Delay between samples in milliseconds (default 10)
   */
  void calibrateWhite(int samples = 50, int delayMs = 10);
  
  /**
   * @brief Calibrate black line
   * @param samples Number of samples to take (default 50)
   * @param delayMs Delay between samples in milliseconds (default 10)
   */
  void calibrateBlack(int samples = 50, int delayMs = 10);
  
  /**
   * @brief Calculate thresholds from white and black calibration values
   */
  void calculateThresholds();
  
  /**
   * @brief Check if calibration has been done
   * @return true if both white and black calibration completed
   */
  bool isCalibrated() const { return _calibrated; }
  
  // ========== PATTERN DETECTION ==========
  
  /**
   * @brief Detect current line pattern
   * @return LinePattern enum value
   */
  LinePattern detectPattern();
  
  /**
   * @brief Get number of sensors currently detecting black line
   * @return Count of active sensors (0-5)
   */
  int getActiveSensorCount();
  
  /**
   * @brief Check if line is lost
   * @return true if no sensors detect the line
   */
  bool isLineLost();
  
  /**
   * @brief Check if T-junction detected
   * @return true if 3-4 sensors active
   */
  bool isTJunction();
  
  /**
   * @brief Check if cross junction detected
   * @return true if 4-5 sensors active
   */
  bool isCrossJunction();
  
  /**
   * @brief Check if end line detected
   * @return true if all sensors active
   */
  bool isEndLine();
  
  // ========== GETTERS ==========
  
  /**
   * @brief Get threshold values
   * @param output Array to store thresholds
   */
  void getThresholds(int* output) const;
  
  /**
   * @brief Get white calibration values
   * @param output Array to store white values
   */
  void getWhiteValues(long* output) const;
  
  /**
   * @brief Get black calibration values
   * @param output Array to store black values
   */
  void getBlackValues(long* output) const;
  
  // ========== DEBUG ==========
  
  /**
   * @brief Print raw sensor values to Serial
   */
  void printRawValues();
  
  /**
   * @brief Print digital sensor values to Serial
   */
  void printDigitalValues();
  
  /**
   * @brief Print calibration table to Serial
   */
  void printCalibrationTable();

  const uint8_t* _pins;
  uint8_t _numSensors;
  
  int* _rawValues;
  bool* _digitalValues;
  long* _whiteValues;
  long* _blackValues;
  int* _thresholds;
  
  bool _calibrated;
  int _lastValidPosition;
  
  void calibrate(long* targetArray, const char* label, int samples, int delayMs);
};

#endif
