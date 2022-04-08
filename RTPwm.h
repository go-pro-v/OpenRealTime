#ifndef RTPWM_H
#define RTPWM_H
#include <Arduino.h>
#include "RTTimer.h"
class RTPwm{
public:
  RTPwm(uint16_t frequence = 100, uint16_t range = 255);
  ~RTPwm();
  void set(uint16_t pwm_value);
  bool get();
  void run();
private:
  RTTimer timer;
  uint16_t period;
  uint16_t range;
  bool state = false;
  uint16_t value = 0;
  uint16_t increment = 0;
};
#endif
