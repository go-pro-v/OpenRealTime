#include "RTPwm.h"
  RTPwm::RTPwm(uint16_t frequence, uint16_t range)
  {
    this->range = range;
    period = 1000000 / frequence;
    timer = RTTimer(period / range);
  }
  RTPwm::~RTPwm(){};
  void RTPwm::set(uint16_t pwm_value)
  {
    value = pwm_value;
    run();
  }
  bool RTPwm::get()
  {
    run();
    return state;
  }
  void RTPwm::run()
  {
    if(timer.isOver())
    {
      increment += timer.sync_reset();
      increment = increment % range;
    }
    bool old_state = state;
    state = increment >= value && value != 0;
  }
