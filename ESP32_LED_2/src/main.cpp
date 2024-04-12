#include <Arduino.h>
int sttold=1;
#define PushButton 21
int led[]={4,16,17,5,19,27,32,34};
int i=0;
void setup() 
{
  pinMode(PushButton,INPUT);
  for (int c=0;c<8;c++)
      {
        pinMode(led[c],OUTPUT);
        digitalWrite(led[c],1);
      }
}
void loop() 
{
  while (1)
  {
    int Push_button_state=digitalRead(PushButton);

    if (Push_button_state==0&&sttold!=Push_button_state)
    {
      delay(5);
      digitalWrite(led[i],0);
      i++;
      delay(200);
    }
    if (i==9
    *
      for (int j=0;j<8;j++)+
      {
        digitalWrite(led[j],1);
      }
      i=0;
    }
    int sttold=Push_button_state;
  }
}