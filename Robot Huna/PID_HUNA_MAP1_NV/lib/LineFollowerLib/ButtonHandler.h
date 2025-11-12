// ButtonHandler.h
#pragma once
#include "BuzzerController.h"
#include "MotorDriver.h"
#include "PIDLineFollower.h"
#include "IRSensorArray.h"

void handleButton(BuzzerController &buzzer, MotorDriver &motor, PIDLineFollower &pid, IRSensorArray &sensor);
