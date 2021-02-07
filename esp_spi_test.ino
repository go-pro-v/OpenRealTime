//#include "esp_spi.h"
#include "HBridge.h"

HBridge bridge;
SPIClass spi;
void setup() {
  spi = SPIClass(VSPI);
  bridge = HBridge(spi, 5);
  /*spi = SPIClass(VSPI);
  spi.begin();
  pinMode(5, OUTPUT);*/
  //spi.beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE1));
}


void update()
{
  bridge.run();
}

void loop() {
  bridge.H1 = !bridge.H1;
  bridge.H2 = !bridge.H2;
  bridge.H3 = !bridge.H3;
  bridge.H4 = !bridge.H4;
  bridge.H5 = !bridge.H5;
  bridge.H6 = !bridge.H6;
  bridge.L1 = bridge.H1;
  bridge.L2 = bridge.H2;
  bridge.L3 = bridge.H3;
  bridge.L4 = bridge.H4;
  bridge.L5 = bridge.H5;
  bridge.L6 = bridge.H6;
  bridge.run();
  update();
}
