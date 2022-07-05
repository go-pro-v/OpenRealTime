#ifndef RTTIMER_H
#define RTTIMER_H
#include <Arduino.h>
class RTTimer{
public:
  RTTimer(unsigned long microsecondes = 0);
  ~RTTimer();
  unsigned long getCurrentClock();
  void run();
  void update();
  bool isTrigged();
  bool isOver();
  void async_reset();
  uint8_t sync_reset();
  static unsigned long deltaTime(unsigned long first, unsigned long second);
private:
  unsigned long startTime;
  unsigned long endTime;
  unsigned long period;
  bool millisecondes;
  bool trigger = false;
  unsigned long memorised_time = 0;
};
#endif
