#ifndef HBRIDGE_H
#define HBRIDGE_H
#include <Arduino.h>
#include <SPI.h>
#include "RTPwm.h"

class HBridge{
public:
  HBridge(uint8_t spi_bus=VSPI, uint8_t slave_pin = 5);
  HBridge(SPIClass & spi_bus, uint8_t slave_pin = 5);
  ~HBridge();
  void run();
public:
  RTPwm H1Pwm = RTPwm();
  RTPwm H2Pwm = RTPwm();
  RTPwm H3Pwm = RTPwm();
  RTPwm H4Pwm = RTPwm();
  RTPwm H5Pwm = RTPwm();
  RTPwm H6Pwm = RTPwm();
  RTPwm L1Pwm = RTPwm();
  RTPwm L2Pwm = RTPwm();
  RTPwm L3Pwm = RTPwm();
  RTPwm L4Pwm = RTPwm();
  RTPwm L5Pwm = RTPwm();
  RTPwm L6Pwm = RTPwm();
private:
  bool H1 = false;
  bool H2 = false;
  bool H3 = false;
  bool H4 = false;
  bool H5 = false;
  bool H6 = false;
  bool L1 = false;
  bool L2 = false;
  bool L3 = false;
  bool L4 = false;
  bool L5 = false;
  bool L6 = false;
  static const int spiClk = 1000000; 
  SPIClass spi;
  uint8_t pin;
  //uint16_t mask = 0b0001111111111110;
  uint16_t data = 0;
  void setData();
  uint16_t place(bool state, uint8_t position);
  void init();
};
#endif