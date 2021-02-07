#ifndef REALTIME_H
#define REALTIME_H
#include <Arduino.h>
#include "RTTimer.h"
void wait(unsigned long millisecondes)
{
  auto timer = RTTimer(millisecondes);
  while(!timer.isOver())
  {
    update();
  }
}
#endif