#include <Arduino.h>

#define led 14
#define PushButton 21
int Push_button_state=0;
void setup() 
{
  pinMode(led,OUTPUT);
  pinMode(PushButton,INPUT);
  digitalWrite(led,1);
}
void loop() 
{
  int i=0,Push_button_state;
 while (1)
  {
    Push_button_state=digitalRead(PushButton);
    if (Push_button_state==0)
    {
      digitalWrite(led,i);
      i++;
      i%=2;
      delay(200);
    }
  }

}