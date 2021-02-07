#include "RTTimer.h"
  RTTimer::RTTimer(unsigned long microsecondes){
    //millisecondes = (microsecondes == 0xFFFFFED8 & microsecondes);
    millisecondes = (microsecondes == ((microsecondes / 1000) * 1000));
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
  RTTimer::~RTTimer(){}
  unsigned long RTTimer::getCurrentTime(){
    if(millisecondes){
      return millis();
    }
    else{
      return micros();
    }
  }
  bool RTTimer::isOver(){
    auto currentTime = getCurrentTime();
    return 
    (endTime >= startTime && (currentTime >= endTime || currentTime < startTime))
    || (endTime < startTime && currentTime < startTime && currentTime >= endTime);
  }
  void RTTimer::async_reset()
  {
    startTime = getCurrentTime();
    endTime = startTime + period;
  }
  uint8_t RTTimer::sync_reset()
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