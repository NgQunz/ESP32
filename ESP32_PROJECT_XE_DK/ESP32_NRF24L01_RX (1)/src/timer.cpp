#include <header.h>

hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
int flag_tx=0;
int flag_joy=0;
int flag_motor=0;
int flag_servo =0;
// int flag_barrier=0;
int flag_cs100a=0;
void IRAM_ATTR check_status()
{
  flag_servo = !flag_servo ;
  flag_motor=1;
  flag_tx=1;
  flag_joy=1;
  flag_cs100a=1;
  //flag_barrier=1;
}

void Setup_Timer()
{
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &check_status, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true); //100ms
  timerAlarmEnable(Timer0_Cfg);
}
