#include "header.h"
int flag_tx=0;
hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
int flag_pause = 0;
int count_timer = 0;

void IRAM_ATTR check_status()
{
  //flag_pause = 1;
  count_timer++;
  if ( count_timer == 1000)
  {
    count_timer = 0;
    flag_tx=1;
  }
}

void Setup_Timer()
{
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &check_status, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true); //100ms
  timerAlarmEnable(Timer0_Cfg);
}
