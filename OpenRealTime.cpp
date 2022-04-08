#include "OpenRealTime.h"
void RTWait(unsigned long millisecondes)
{
  auto timer = RTTimer(millisecondes);
  while(!timer.isOver())
  {
    update();
  }
}
