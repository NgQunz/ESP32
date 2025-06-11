#include<Wire.h> 
void setup() 
{
  Wire.begin();
}
void loop() {
  Wire.beginTransmission(0x27);
  Wire.write(0b11111111);
  Wire.endTransmission();
  delay(200);
  Wire.beginTransmission(0x27);
  Wire.write(0b00000000);
  Wire.endTransmission();
  delay(200);
S
}