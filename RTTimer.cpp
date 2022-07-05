#include "RTTimer.h"
#include <limits.h>

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
    update();
    async_reset();
  }
  RTTimer::~RTTimer(){}

  unsigned long RTTimer::getCurrentClock(){
    return memorised_time;
  }
  void RTTimer::update()
  {
    if(millisecondes){
      memorised_time = millis();
    }
    else{
      memorised_time = micros();
    }
  }
  void RTTimer::run()
  {
    update();
    if(isOver())
    {
      sync_reset();
      trigger = true;
    }
    else
    {
      trigger = false;
    }
  }
  bool RTTimer::isTrigged()
  {
    return trigger;
  }

  unsigned long RTTimer::deltaTime(unsigned long first, unsigned long second)
  {
    return second - first;
    /*if(first <= second)
    {
      return second - first;
    }
    else
    {
      return 1 + first + (ULONG_MAX - second);
    }*/
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
    }
  }