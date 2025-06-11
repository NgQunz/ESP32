/*
 ****************************************************************************
 * @file	df_mlx90614.h                                                           *
 * @author	Nguyen Thi Quynh                                             *
 * @date	15/01/2025
 * @ver 1.0                                                       *
 ****************************************************************************
 */
#include <df_mlx90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
DFRobot_Heartrate Heartrate(DIGITAL_MODE); // ANALOG_MODE or DIGITAL_MODE
float tempc;
int heartrate;
void df_mlx_setup()
{
    pinMode(heartratePin, INPUT);
    while (!Serial)
        ;
    if (!mlx.begin())
    {
        Serial.println("Error connecting to MLX sensor. Check wiring.");
        while (1)
            ;
    };
}

void updateSimulatedData()
{
    Heartrate.getValue(heartratePin); // A1 foot sampled values
    heartrate = Heartrate.getRate();  // Get heart rate value
    tempc = mlx.readObjectTempC();
    if (heartrate)
    {
        Serial.println(heartrate);
    }
    delay(100);
}

void Warning()
{
    if (tempc <= 35)
    {
        Serial.println("Warning: Tempcerature is too low");
    }
    if (tempc >= 38)
    {
        Serial.println("Warning: Tempcerature is too high");
    }
    if (heartrate <= 60)
    {
        Serial.println("Warning: Heart rate is too low");
    }
    if (heartrate >= 100)
    {
        Serial.println("Warning: Heart rate is too high");
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/