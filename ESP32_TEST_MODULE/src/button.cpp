#include "header.h"

int sttnew35 = 1;
int sttnew26 = 1;
int sttnew32 = 1;
int sttnew33 = 1;
int sttnew34 = 1;

void Declare_Button()
{
  pinMode(35, INPUT);
  pinMode(26, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
}

int Press_Button35()
{
  int sttold = sttnew35;
  sttnew35 = digitalRead(35);
  if ( sttold == 1 && sttnew35 == 0)
  {
    return 1;
  }
  return 0;
}

int Press_Button26()
{
  int sttold = sttnew26;
  sttnew26 = digitalRead(26);
  if ( sttold == 1 && sttnew26 == 0)
  {
    return 1;
  }
  return 0;
}

int Press_Button32()
{
  int sttold = sttnew32;
  sttnew32 = digitalRead(32);
  if ( sttold == 1 && sttnew32 == 0)
  {
    return 1;
  }
  return 0;
}

int Press_Button33()
{
  int sttold = sttnew33;
  sttnew33 = digitalRead(33);
  if ( sttold == 1 && sttnew33 == 0)
  {
    return 1;
  }
  return 0;
}

int Press_Button34()
{
  int sttold = sttnew34;
  sttnew34 = digitalRead(34);
  if ( sttold == 1 && sttnew34 == 0)
  {
    return 1;
  }
  return 0;
}
