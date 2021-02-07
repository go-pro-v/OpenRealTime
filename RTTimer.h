#ifndef RTTIMER_H
#define RTTIMER_H
#include <Arduino.h>
class RTTimer{
public:
  RTTimer(unsigned long microsecondes = 0);
  ~RTTimer();
  unsigned long getCurrentTime();
  bool isOver();
  void async_reset();
  uint8_t sync_reset();
private:
  unsigned long startTime;
  unsigned long endTime;
  unsigned long period;
  bool millisecondes;
};
#endif
