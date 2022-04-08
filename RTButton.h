#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
class RTButton
{
  public:
  RTButton(uint8_t pin, bool lowState);
  void run();
  bool state();
  bool previousState();
  bool isTrigged();
  private:
  const int debouceValue = 10;
  const int debouceTimerTimeOut = debouceValue + 1000;
  bool previousRealButtonState;
  unsigned long lastRealButtonChangeTime;
  bool currentButtonState = LOW;
  bool previousButtonState = currentButtonState;
  bool invertState;
  bool readInput();
  uint8_t pinInput;
};
#endif
