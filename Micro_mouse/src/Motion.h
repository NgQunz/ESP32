#ifndef __MOTION_H__
#define __MOTION_H__

#include <Arduino.h>
#include <vl53lx.h>

class Motion
{
private:
  int speed_max = 60;
  int speed_tb = 0;
  int speed_target = 50;
  int state = 1;
  int P;
  int D;
  int I;

  int Pvalue = 6;
  int Dvalue = 0;
  int Ivalue = 0;
  int previous_error;

public:
  float target;
  float error;
  int left = 70;
  void set_pin();
  void run18cm();
  void T();
  void run_pid();
  void set_speed(int lsp, int rsp);
  void set_error(float now);
  void set_target(float first_read_from_imu);

  void set_state(int state);
  void set_speed_tb(int s_tb);

  float get_error();
  float get_target();
  void TLUI();
  void run_straight();
  void LUI();
  void TRAI();
  void PHAI();

  void back();
  void speed_increase();
  void stop();
};

#endif
