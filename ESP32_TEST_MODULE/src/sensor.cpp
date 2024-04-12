#include "header.h"

// ntc
const double VCC = 3.3; 
const double R2 = 10000;
const double adc_resolution = 4095; 
const double A = 0.001129148;
const double B = 0.000234125;
const double C = 0.0000000876741; 
double t = 0; 
int gas = 0;

void Temp ()
{
  double Vout, Rth, adc_value; 
  adc_value = analogRead(25);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;
  t = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));  
  t = t - 273.15;
  printf("%.2df\n",t);
}

void Declare_Sensor()
{
    pinMode(PIN_LIGHT, INPUT);
    pinMode(PIN_PROX, INPUT);
}

void Gas()
{
  int x = analogRead(PIN_GAS) - 2095; // 2095 ~ 4095
  if ( x <= 0)
  {
    gas = 0;
  }
  else 
  {
    gas = x / 20;
  }
}

int Night()
{
    if ( digitalRead(PIN_LIGHT) == 1) // trời tối
    {
        return 1;
    }
    return 0;
}
