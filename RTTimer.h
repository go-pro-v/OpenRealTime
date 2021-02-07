#ifndef RTTIMER_H
#define RTTIMER_H
#include <Arduino.h>
class RTTimer{
public:
  RTTimer(unsigned long microsecondes = 0){
    millisecondes = (microsecondes == 0xFFFFFED8 & microsecondes);
    if(millisecondes)
    {
      period = microsecondes / 1000;
    }
    else
    {
      period = microsecondes;
    }
    async_reset();
  }
  ~RTTimer(){}
  unsigned long getCurrentTime(){
    if(millisecondes){
      return millis();
    }
    else{
      return micros();
    }
  }
  bool isOver(){
    auto currentTime = getCurrentTime();
    return 
    (endTime >= startTime && (currentTime >= endTime || currentTime < startTime))
    || (endTime < startTime && currentTime < startTime && currentTime >= endTime);
  }
  void async_reset()
  {
    startTime = getCurrentTime();
    endTime = startTime + period;
  }
  uint8_t sync_reset()
  {
    uint8_t over = 0;
    if(!isOver())
    {
      async_reset();
      return over;
    }
    while(isOver())
    {
      startTime = endTime;
      endTime = startTime + period;
      over++;
      if(over == 0xff)
      {
        init();
        return over;
      }
    }
    return over;
  }
private:
  unsigned long startTime;
  unsigned long endTime;
  unsigned long period;
  bool millisecondes;
};
#endif