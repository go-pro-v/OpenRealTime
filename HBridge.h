#ifndef HBRIDGE_H
#define HBRIDGE_H
#include <Arduino.h>
#include <SPI.h>

class HBridge{
public:
  HBridge(uint8_t spi_bus=VSPI, uint8_t slave_pin = 5){
    spi = SPIClass(spi_bus);
    spi.begin();
    pin = slave_pin;
    pinMode(pin, OUTPUT);
  }
  HBridge(SPIClass & spi_bus, uint8_t slave_pin = 5){
    spi = spi_bus;
    spi.begin();
    pin = slave_pin;
    pinMode(pin, OUTPUT);
  }
  ~HBridge(){}
  void run(){
    setData();
    spi.beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE1));
    digitalWrite(pin, LOW);
    spi.transfer16(data);
    digitalWrite(pin, HIGH);
    spi.endTransaction();
    
  }
public:
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
private:
  static const int spiClk = 1000000; 
  SPIClass spi;
  uint8_t pin;
  //uint16_t mask = 0b0001111111111110;
  uint16_t data = 0;
  void setData()
  {
    data = 
      place(L1,1) |
      place(H1,2) |
      place(L2,3) |
      place(H2,4) |
      place(L3,5) |
      place(H3,6) |
      place(L4,7) |
      place(H4,8) |
      place(L5,9) |
      place(H5,10) |
      place(L6,11) |
      place(H6,12) ;
  }
  uint16_t place(bool state, uint8_t position)
  {
    return state?(0x0001 << position):0x0000;
  }
};
#endif