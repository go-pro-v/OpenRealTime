#ifndef RTPWM_H
#define RTPWM_H
#include <Arduino.h>
#include "RTTimer.h"
class PWM{
public:
  RTPwm(uint16_t frequence = 100)
  {
    period = 1000 / frequence;
    timer = RTTimer(period);
  }
  ~RTPwm(){}
  void set(uint16_t pwm_value)
  {
    value = pwm_value;
    run();
  }
  bool get()
  {
    run();
    return state;
  }
  void run()
  {
    if(timer.isOver())
    {
      increment += timer.sync_reset();
    }
    state = increment >= value && value != 0;
  }
private:
  RTTimer timer;
  uint16_t period;
  bool state = false;
  uint16_t value = 0;
  uint16_t increment = 0;
};
#endif