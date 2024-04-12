#include <Arduino.h>

int led[] ={4, 16, 17};
void setup() {
  for (int i=0;i<3;i++)
  {
    pinMode(led[i],OUTPUT);
  }
}

void loop() 
{ int j=1,i=0;
  while (1)
  {
    delay(50);
    i++;
    if (i%5==0)
    {
      digitalWrite(led[0], j);
      j=(j+1)%2;
    }
    if (i%10==0)
    {
      digitalWrite(led[1], j);
      j=(j+1)%2;
    }
    if (i%20==0)
    {
      digitalWrite(led[2], j);
      j=(j+1)%2;
    }
    digitalRead();
  }
}