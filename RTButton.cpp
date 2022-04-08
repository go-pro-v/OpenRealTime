#include "RTButton.h"
RTButton::RTButton(uint8_t pin, bool lowState)
{
  pinInput = pin;
  pinMode(pinInput, INPUT);
  invertState = lowState;
  static auto lastRealButtonChangeTime = millis();
  static auto previousRealButtonState = readInput();
}

bool RTButton::readInput()
{
  return invertState ? !digitalRead(pinInput) : digitalRead(pinInput);
}

void RTButton::run()
{
  
// gestion du bouton avec debounce
  auto currentTime = millis();
  auto currentRealButtonState = readInput();
  
  if(lastRealButtonChangeTime + debouceTimerTimeOut < currentTime) // ne pas laisser le timer viellir trop longtemps pour éviter un overflow trop important
  {
    lastRealButtonChangeTime = currentTime - debouceTimerTimeOut;
  }

  previousButtonState = currentButtonState;
  if(currentButtonState != currentRealButtonState && lastRealButtonChangeTime + debouceValue < currentTime)
  {
    currentButtonState = currentRealButtonState;
  }

}
bool RTButton::state()
{
  return currentButtonState;
}
bool RTButton::previousState()
{
  return previousButtonState;
}
bool RTButton::isTrigged()
{
  return state() && (state() != previousState());
}
