#include "IRSensorArray.h"

IRSensorArray::IRSensorArray(const uint8_t* pins, uint8_t numSensors)
  : _pins(pins), _numSensors(numSensors), _calibrated(false)
{
  _rawValues = new int[numSensors];
  _digitalValues = new bool[numSensors];
  _whiteValues = new long[numSensors];
  _blackValues = new long[numSensors];
  _thresholds = new int[numSensors];
  
  _lastValidPosition = (numSensors - 1) * 100 / 2; // Center position
  
  // Initialize arrays
  for (uint8_t i = 0; i < numSensors; i++) {
    _rawValues[i] = 0;
    _digitalValues[i] = false;
    _whiteValues[i] = 0;
    _blackValues[i] = 0;
    _thresholds[i] = 512;
  }
}

void IRSensorArray::begin() {
  for (uint8_t i = 0; i < _numSensors; i++) {
    pinMode(_pins[i], INPUT);
  }
}

// ========== SENSOR READING ==========

void IRSensorArray::readRaw(int* output) {
  for (uint8_t i = 0; i < _numSensors; i++) {
    output[i] = analogRead(_pins[i]);
    delay(1); // Small delay for ADC stability
  }
}

void IRSensorArray::readDigital(bool* output) {
  readRaw(_rawValues);
  
  for (uint8_t i = 0; i < _numSensors; i++) {
    // Black line has LOWER value than threshold
    output[i] = (_rawValues[i] < _thresholds[i]);
  }
}
// Thêm sau hàm readDigital() hiện tại

byte IRSensorArray::getDigitalByte() {
  readRaw(_rawValues);
  
  byte result = 0;
  
  // Build byte: Bit 4 = Sensor 0 (Left), Bit 0 = Sensor 4 (Right)
  for (uint8_t i = 0; i < _numSensors; i++) {
    if (_rawValues[i] < _thresholds[i]) {
      // Set bit tương ứng
      result |= (1 << (_numSensors - 1 - i));
    }
  }
  
  return result;
}
int IRSensorArray::getPosition() {
  if (!_calibrated) return _lastValidPosition;
  
  readDigital(_digitalValues);
  
  // Check if any sensor detects line
  bool lineDetected = false;
  for (uint8_t i = 0; i < _numSensors; i++) {
    if (_digitalValues[i]) {
      lineDetected = true;
      break;
    }
  }
  
  if (!lineDetected) {
    return _lastValidPosition; // Return last known position
  }
  
  // Weighted average calculation
  long weightedSum = 0;
  long sensorCount = 0;
  
  for (uint8_t i = 0; i < _numSensors; i++) {
    if (_digitalValues[i]) {
      weightedSum += (long)i * 100;
      sensorCount++;
    }
  }
  
  if (sensorCount > 0) {
    int position = weightedSum / sensorCount;
    _lastValidPosition = position;
    return position;
  }
  
  return _lastValidPosition;
}

// ========== CALIBRATION ==========

void IRSensorArray::calibrateWhite(int samples, int delayMs) {
  calibrate(_whiteValues, "WHITE", samples, delayMs);
}

void IRSensorArray::calibrateBlack(int samples, int delayMs) {
  calibrate(_blackValues, "BLACK", samples, delayMs);
}

void IRSensorArray::calculateThresholds() {
  Serial.println(F("\n=== CALCULATING THRESHOLDS ==="));
  
  for (uint8_t i = 0; i < _numSensors; i++) {
    _thresholds[i] = (_whiteValues[i] + _blackValues[i]) / 2;
  }
  
  _calibrated = true;
  
  Serial.println(F("\nSensor | White | Black | Threshold"));
  Serial.println(F("-------|-------|-------|----------"));
  for (uint8_t i = 0; i < _numSensors; i++) {
    Serial.print(F("  S"));
    Serial.print(i);
    Serial.print(F("   | "));
    Serial.print(_whiteValues[i]);
    if (_whiteValues[i] < 100) Serial.print(F(" "));
    if (_whiteValues[i] < 10) Serial.print(F(" "));
    Serial.print(F("  | "));
    Serial.print(_blackValues[i]);
    if (_blackValues[i] < 100) Serial.print(F(" "));
    if (_blackValues[i] < 10) Serial.print(F(" "));
    Serial.print(F("  | "));
    Serial.println(_thresholds[i]);
  }
  
  Serial.println(F("\n✓ Calibration complete!\n"));
}

void IRSensorArray::calibrate(long* targetArray, const char* label, int samples, int delayMs) {
  Serial.print(F("\n=== CALIBRATING "));
  Serial.print(label);
  Serial.println(F(" ==="));
  Serial.println(F("Place sensors on target surface..."));
  Serial.println(F("Starting in 2 seconds...\n"));
  
  delay(2000);
  
  // Initialize
  for (uint8_t i = 0; i < _numSensors; i++) {
    targetArray[i] = 0;
  }
  
  Serial.print(F("Reading "));
  Serial.print(samples);
  Serial.println(F(" samples..."));
  Serial.println(F("Sample | S0    S1    S2    S3    S4"));
  Serial.println(F("-------|---------------------------"));
  
  // Take samples
  for (int sample = 0; sample < samples; sample++) {
    readRaw(_rawValues);
    
    // Print sample
    Serial.print(F("  "));
    if (sample < 10) Serial.print(F(" "));
    Serial.print(sample + 1);
    Serial.print(F("   | "));
    
    for (uint8_t i = 0; i < _numSensors; i++) {
      if (_rawValues[i] < 100) Serial.print(F(" "));
      if (_rawValues[i] < 10) Serial.print(F(" "));
      Serial.print(_rawValues[i]);
      Serial.print(F("  "));
      
      targetArray[i] += _rawValues[i];
    }
    Serial.println();
    
    delay(delayMs);
  }
  
  // Calculate averages
  Serial.print(F("\n--- Average "));
  Serial.print(label);
  Serial.print(F(" values: "));
  for (uint8_t i = 0; i < _numSensors; i++) {
    targetArray[i] /= samples;
    Serial.print(targetArray[i]);
    Serial.print(F(" "));
  }
  Serial.println(F("\n✓ Done!\n"));
}

// ========== PATTERN DETECTION ==========

LinePattern IRSensorArray::detectPattern() {
  readDigital(_digitalValues);
  int activeCount = getActiveSensorCount();
  
  if (activeCount == 0) {
    return PATTERN_LOST;
  }
  
  if (activeCount == _numSensors) {
    return PATTERN_END_LINE;
  }
  
  if (activeCount >= 4) {
    return PATTERN_CROSS_JUNCTION;
  }
  
  if (activeCount == 3) {
    return PATTERN_T_JUNCTION;
  }
  
  // Check if leftmost sensors active (sharp left)
  if (_digitalValues[0] && _digitalValues[1] && !_digitalValues[_numSensors - 1]) {
    return PATTERN_LEFT_TURN;
  }
  
  // Check if rightmost sensors active (sharp right)
  if (_digitalValues[_numSensors - 1] && _digitalValues[_numSensors - 2] && !_digitalValues[0]) {
    return PATTERN_RIGHT_TURN;
  }
  
  return PATTERN_STRAIGHT;
}

int IRSensorArray::getActiveSensorCount() {
  int count = 0;
  for (uint8_t i = 0; i < _numSensors; i++) {
    if (_digitalValues[i]) count++;
  }
  return count;
}

bool IRSensorArray::isLineLost() {
  return (getActiveSensorCount() == 0);
}

bool IRSensorArray::isTJunction() {
  int count = getActiveSensorCount();
  return (count == 3 || count == 4);
}

bool IRSensorArray::isCrossJunction() {
  int count = getActiveSensorCount();
  return (count >= 4 && count < _numSensors);
}

bool IRSensorArray::isEndLine() {
  return (getActiveSensorCount() == _numSensors);
}

// ========== GETTERS ==========

void IRSensorArray::getThresholds(int* output) const {
  for (uint8_t i = 0; i < _numSensors; i++) {
    output[i] = _thresholds[i];
  }
}

void IRSensorArray::getWhiteValues(long* output) const {
  for (uint8_t i = 0; i < _numSensors; i++) {
    output[i] = _whiteValues[i];
  }
}

void IRSensorArray::getBlackValues(long* output) const {
  for (uint8_t i = 0; i < _numSensors; i++) {
    output[i] = _blackValues[i];
  }
}

// ========== DEBUG ==========

void IRSensorArray::printRawValues() {
  readRaw(_rawValues);
  
  Serial.print(F("Raw: "));
  for (uint8_t i = 0; i < _numSensors; i++) {
    Serial.print(_rawValues[i]);
    Serial.print(F("\t"));
  }
  Serial.println();
}

void IRSensorArray::printDigitalValues() {
  readDigital(_digitalValues);
  
  Serial.print(F("Digital: "));
  for (uint8_t i = 0; i < _numSensors; i++) {
    Serial.print(_digitalValues[i] ? '1' : '0');
    if (i < _numSensors - 1) Serial.print(F(" "));
  }
  Serial.println();
}

void IRSensorArray::printCalibrationTable() {
  Serial.println(F("\n=== CALIBRATION TABLE ==="));
  Serial.println(F("Sensor | White | Black | Threshold"));
  Serial.println(F("-------|-------|-------|----------"));
  for (uint8_t i = 0; i < _numSensors; i++) {
    Serial.print(F("  S"));
    Serial.print(i);
    Serial.print(F("   | "));
    Serial.print(_whiteValues[i]);
    Serial.print(F("  | "));
    Serial.print(_blackValues[i]);
    Serial.print(F("  | "));
    Serial.println(_thresholds[i]);
  }
  Serial.println();
}

