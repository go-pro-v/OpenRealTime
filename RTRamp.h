#ifndef RTRAMP_H
#define RTRAMP_H
#include <Arduino.h>
#include <RTTimer.h>
class RTRamp{
public:
  RTRamp(uint16_t ms_to_max = 0, uint16_t range = 255, uint16_t initValue = 0);
  void set(uint16_t pwm_value);
  uint16_t get();
  void run();
  void start();
  bool changed();
private:
  RTTimer m_step_timer;
  uint16_t m_ms_to_max = 0;
  uint16_t m_range = 255;
  uint16_t m_target = 0;
  uint16_t m_value = 0;
  bool m_changed = false;
};
#endif
