#include "RTButton.h"
RTButton::RTButton(uint8_t pin, bool lowState, int debouce)
{
  pinInput = pin;
  pinMode(pinInput, INPUT);
  invertState = lowState;
  debouceValue = debouce;
  debouceTimerTimeOut = debouceValue + 1000;
  lastRealButtonChangeTime = millis();
  previousRealButtonState = readInput();
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

  if(previousRealButtonState != currentRealButtonState)
  {
    lastRealButtonChangeTime = currentTime;
  }
  // la gestion des overflow n'est pas encore parfaite, mais les cas foireux sont rares et isolés. 
  if(
    lastRealButtonChangeTime + debouceTimerTimeOut < currentTime
    || (lastRealButtonChangeTime > currentTime && lastRealButtonChangeTime + debouceTimerTimeOut > currentTime)
  ) // ne pas laisser le timer viellir trop longtemps pour éviter un overflow trop important
  {
    lastRealButtonChangeTime = currentTime - debouceTimerTimeOut;
  }

  previousButtonState = currentButtonState;
  if(currentButtonState != currentRealButtonState && 
      (
        lastRealButtonChangeTime + debouceValue < currentTime
        || (lastRealButtonChangeTime > currentTime && lastRealButtonChangeTime + debouceValue > currentTime)
      )
    )
  {
    currentButtonState = currentRealButtonState;
  }

  previousRealButtonState = currentRealButtonState;
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
