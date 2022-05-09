#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
class RTButton
{
  public:
  RTButton(uint8_t pin, bool lowState, int debouce = 10);
  void run();
  bool state();
  bool previousState();
  bool isTrigged();
  private:
  int debouceValue = 10;
  int debouceTimerTimeOut = debouceValue + 1000;
  bool previousRealButtonState;
  unsigned long lastRealButtonChangeTime;
  bool currentButtonState = LOW;
  bool previousButtonState = currentButtonState;
  bool invertState;
  bool readInput();
  uint8_t pinInput;
};
#endif
