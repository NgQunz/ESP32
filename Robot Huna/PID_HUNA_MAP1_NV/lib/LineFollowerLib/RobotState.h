// RobotState.h
#pragma once
enum RobotState
{
    STATE_IDLE,
    STATE_CALIB_WHITE,
    STATE_CALIB_BLACK,
    STATE_RUNNING
};
extern RobotState currentState;
extern int buttonPressCount;
