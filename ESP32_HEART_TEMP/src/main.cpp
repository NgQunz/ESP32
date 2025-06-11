#include "DFRobot_Heartrate.h"
#include <Adafruit_MLX90614.h>
#include <SPI.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#define heartratePin 32

DFRobot_Heartrate heartrate(DIGITAL_MODE); // ANALOG_MODE or DIGITAL_MODE

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  if (!mlx.begin())
  {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };
}

void loop()
{
  uint8_t rateValue;
  heartrate.getValue(heartratePin); // A1 foot sampled values
  rateValue = heartrate.getRate();  // Get heart rate value
  if (rateValue)
  {
    Serial.println(rateValue);
    Serial.print(mlx.readObjectTempC());
  }
  delay(20);
}
