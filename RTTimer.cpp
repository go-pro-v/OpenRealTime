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

  unsigned long RTTimer::getCurrentClock(){
    if(millisecondes){
      return millis();
    }
    else{
      return micros();
    }
  }

  unsigned long RTTimer::deltaTime(unsigned long first, unsigned long second)
  {
    if(first <= second)
    {
      return second - first;
    }
    else
    {
      return first + (0xffffffff - second);
    }
  }
  bool RTTimer::isOver(){
    unsigned long currentTime = getCurrentClock();
    return deltaTime(startTime, currentTime) >= period;
  }
  void RTTimer::async_reset()
  {
    startTime = getCurrentClock();
    endTime = startTime + period;
  }
  uint8_t RTTimer::sync_reset()
  {
    if(!isOver())
    {
      async_reset();
      return 0;
    }
    else
    {
      unsigned long currentTime = getCurrentClock();
      unsigned long delta = deltaTime(startTime, currentTime);
      unsigned long offset = delta % period;
      unsigned long cycles = (delta - offset) / period;
      startTime += period * cycles;
      endTime = startTime + period;
      return min((unsigned long)(0xff), cycles);
    }  }