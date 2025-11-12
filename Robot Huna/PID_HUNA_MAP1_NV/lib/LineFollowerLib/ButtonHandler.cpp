// ButtonHandler.cpp
#include "ButtonHandler.h"
#include "RobotState.h"
#include "LEDController.h"
#include "CalibrationManager.h"
#include "PinConfig.h"
#define DEBOUNCE_DELAY 50
bool lastButtonState = LOW;

void handleButton(BuzzerController &buzzer, MotorDriver &motor, PIDLineFollower &pid, IRSensorArray &sensor)
{
    int reading = digitalRead(BUTTON_PIN);
    static unsigned long lastChange = 0;

    if (reading != lastButtonState)
        lastChange = millis();

    if ((millis() - lastChange) > DEBOUNCE_DELAY && reading == HIGH && lastButtonState == LOW)
    {
        buttonPressCount++;
        buzzer.beep(2000, 100);

        switch (currentState)
        {
        case STATE_IDLE:
            currentState = STATE_CALIB_WHITE;
            Serial.println(F("STATE: CALIB_WHITE"));
            ledSteady();
            break;
        case STATE_CALIB_WHITE:
            calibrateWhiteWithLED(sensor);
            currentState = STATE_CALIB_BLACK;
            buzzer.success();
            break;
        case STATE_CALIB_BLACK:
            calibrateBlackWithLED(sensor);
            sensor.calculateThresholds();
            currentState = STATE_RUNNING;
            pid.setEnabled(true);
            buzzer.startup();
            ledOff();
            break;
        case STATE_RUNNING:
            pid.setEnabled(false);
            motor.stop();
            currentState = STATE_IDLE;
            buttonPressCount = 0;
            buzzer.warning();
            ledSteady();
            break;
        }
    }
    lastButtonState = reading;
}
