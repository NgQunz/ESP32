#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <Arduino.h>

/**
 * @brief Non-blocking buzzer controller
 * 
 * Features:
 * - Play single beep with frequency and duration
 * - Play sequence of multiple notes
 * - Non-blocking operation (must call update() in loop)
 * - Preset melodies (success, error, warning, startup)
 */
class BuzzerController {
public:
  /**
   * @brief Constructor
   * @param pin Buzzer pin (digital output)
   */
  BuzzerController(uint8_t pin);
  
  /**
   * @brief Initialize buzzer pin
   */
  void begin();
  
  /**
   * @brief Update buzzer state (MUST be called continuously in loop)
   * 
   * This handles the non-blocking tone generation.
   * Call this in your main loop() function.
   */
  void update();
  
  /**
   * @brief Play a single beep
   * @param frequency Frequency in Hz (100-10000)
   * @param duration Duration in milliseconds
   */
  void beep(int frequency, unsigned long duration);
  
  /**
   * @brief Play a sequence of notes
   * @param frequencies Array of frequencies in Hz
   * @param durations Array of durations in milliseconds
   * @param count Number of notes in the sequence
   * 
   * Example:
   *   int freq[] = {1000, 1500, 2000};
   *   unsigned long dur[] = {200, 200, 300};
   *   buzzer.playSequence(freq, dur, 3);
   */
  void playSequence(const int* frequencies, const unsigned long* durations, int count);
  
  /**
   * @brief Stop current sound
   */
  void stop();
  
  /**
   * @brief Check if buzzer is currently playing
   * @return true if playing
   */
  bool isPlaying() const { return _playing; }
  
  // ========== PRESET MELODIES ==========
  
  /**
   * @brief Play success melody (2 short beeps)
   */
  void success();
  
  /**
   * @brief Play error melody (1 long low beep)
   */
  void error();
  
  /**
   * @brief Play warning melody (3 fast beeps)
   */
  void warning();
  
  /**
   * @brief Play startup melody (ascending tones)
   */
  void startup();


  uint8_t _pin;
  
  // Single beep state
  bool _playing;
  int _frequency;
  unsigned long _duration;
  unsigned long _startTime;
  bool _toneState;
  unsigned long _lastToggle;
  unsigned long _halfPeriod; // Half of the period in microseconds
  
  // Sequence state
  bool _playingSequence;
  const int* _seqFrequencies;
  const unsigned long* _seqDurations;
  int _seqCount;
  int _seqIndex;
  unsigned long _seqStartTime;
  
  void startBeep(int frequency, unsigned long duration);
  void updateSequence();
};

#endif
