#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

// ================== MOTOR PINS ==================
#define ML_DIR 1        // Left motor direction
#define MR_DIR 0        // Right motor direction
#define ML_PWM 8        // Left motor PWM (SoftPWM)
#define MR_PWM 6        // Right motor PWM (SoftPWM)

// ================== SENSOR PINS ==================
// 5 IR sensors from LEFT to RIGHT
#define SENSOR_0 A0     // Leftmost sensor
#define SENSOR_1 A1
#define SENSOR_2 A9     // Center sensor
#define SENSOR_3 A2
#define SENSOR_4 A3     // Rightmost sensor

// ================== UI PINS ==================
#define BUTTON_PIN 15   // Calibration/Start button
#define LED_PIN 16      // Status LED
#define BUZZER_PIN 5    // Buzzer (passive)

// ================== CONFIGURATION ==================
#define NUM_SENSORS 5   // Number of IR sensors

#endif
