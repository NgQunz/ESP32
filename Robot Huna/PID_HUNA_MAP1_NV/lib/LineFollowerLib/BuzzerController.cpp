#include "BuzzerController.h"

BuzzerController::BuzzerController(uint8_t pin)
  : _pin(pin), _playing(false), _frequency(0), _duration(0),
    _startTime(0), _toneState(false), _lastToggle(0), _halfPeriod(0),
    _playingSequence(false), _seqFrequencies(nullptr), 
    _seqDurations(nullptr), _seqCount(0), _seqIndex(0), _seqStartTime(0)
{
}

void BuzzerController::begin() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void BuzzerController::update() {
  // Handle sequence playback
  if (_playingSequence) {
    updateSequence();
    return;
  }
  
  // Handle single beep
  if (!_playing) return;
  
  unsigned long now = millis();
  
  // Check if duration expired
  if (now - _startTime >= _duration) {
    stop();
    return;
  }
  
  // Generate square wave (toggle at half period)
  unsigned long nowMicros = micros();
  if (nowMicros - _lastToggle >= _halfPeriod) {
    _lastToggle = nowMicros;
    _toneState = !_toneState;
    digitalWrite(_pin, _toneState);
  }
}

void BuzzerController::beep(int frequency, unsigned long duration) {
  startBeep(frequency, duration);
}

void BuzzerController::playSequence(const int* frequencies, const unsigned long* durations, int count) {
  if (count <= 0) return;
  
  _seqFrequencies = frequencies;
  _seqDurations = durations;
  _seqCount = count;
  _seqIndex = 0;
  _playingSequence = true;
  
  // Start first note
  startBeep(frequencies[0], durations[0]);
  _seqStartTime = millis();
}

void BuzzerController::stop() {
  _playing = false;
  _playingSequence = false;
  digitalWrite(_pin, LOW);
}

void BuzzerController::startBeep(int frequency, unsigned long duration) {
  _frequency = constrain(frequency, 100, 10000);
  _duration = duration;
  _startTime = millis();
  _playing = true;
  _toneState = false;
  _lastToggle = micros();
  
  // Calculate half period in microseconds
  // halfPeriod = 1 / (2 * frequency) seconds = 1000000 / (2 * frequency) microseconds
  _halfPeriod = 500000UL / _frequency;
  
  digitalWrite(_pin, LOW);
}

void BuzzerController::updateSequence() {
  if (!_playingSequence) return;
  
  unsigned long now = millis();
  
  // Check if current note finished
  if (now - _seqStartTime >= _seqDurations[_seqIndex]) {
    _seqIndex++;
    
    // Check if sequence finished
    if (_seqIndex >= _seqCount) {
      stop();
      return;
    }
    
    // Start next note
    startBeep(_seqFrequencies[_seqIndex], _seqDurations[_seqIndex]);
    _seqStartTime = now;
  }
  
  // Update current tone
  if (_playing) {
    unsigned long nowMicros = micros();
    if (nowMicros - _lastToggle >= _halfPeriod) {
      _lastToggle = nowMicros;
      _toneState = !_toneState;
      digitalWrite(_pin, _toneState);
    }
  }
}

// ========== PRESET MELODIES ==========

void BuzzerController::success() {
  static const int freq[] = {1000, 2200};
  static const unsigned long dur[] = {150, 200};
  playSequence(freq, dur, 2);
}

void BuzzerController::error() {
  beep(500, 800);
}

void BuzzerController::warning() {
  static const int freq[] = {1500, 1500, 1500};
  static const unsigned long dur[] = {100, 100, 100};
  playSequence(freq, dur, 3);
}

void BuzzerController::startup() {
  static const int freq[] = {1000, 1500, 2000, 2500};
  static const unsigned long dur[] = {150, 150, 150, 300};
  playSequence(freq, dur, 4);
}
