/*
 ****************************************************************************
 * @file	df_mlx90614.h                                                             *
 * @author	Nguyen Thi Quynh                                             *
 * @date	15/01/2025
 * @ver 1.0                                                       *
 ****************************************************************************
 */

#include "DFRobot_Heartrate.h"
#include <Adafruit_MLX90614.h>
#include <SPI.h>

#define heartratePin 32
extern float tempc;
extern int heartrate;
void df_mlx_setup();

// Function to simulate tempcerature and read heart rate
void updateSimulatedData();

void Warning();

/********************************* END OF FILE ********************************/
/******************************************************************************/