#include<header.h>
NewPing sonar = NewPing(34, 35, MAX_DISTANCE );
static bool sonarInitialized= false;
int flag_barrier=0;
void CS100A_SETUP()
{
   Serial.begin(115200); 
   sonar = NewPing(34, 35, MAX_DISTANCE);
   sonarInitialized = true;
}
float distance=0;
void CS100A_RUN()
{
   if(flag_cs100a)
   {
      if (!sonarInitialized) 
      {
         CS100A_SETUP();
      }
      distance = sonar.ping_cm();
      if(distance >=0 && distance <= 8) 
      {
         flag_barrier=1;
      }
      else
      {
         flag_barrier=0;
      }
      flag_cs100a = 0;
   }
}
