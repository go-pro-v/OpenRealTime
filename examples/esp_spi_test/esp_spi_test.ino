#include <HBridge.h>
#include <OpenRealTime.h>

HBridge bridge;
SPIClass spi;
void setup() {
  Serial.begin(9600);
  spi = SPIClass(VSPI);
  bridge = HBridge(spi, 5);
  pinMode(1,OUTPUT);
}


void update()
{
  bridge.run();
  digitalWrite(1,bridge.H1Pwm.get());
}
uint8_t v = 0;
void loop() {
  /*bridge.H1 = !bridge.H1;
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
  bridge.L6 = bridge.H6;*/
  bridge.H1Pwm.set(v);
  bridge.H2Pwm.set(v);
  bridge.H3Pwm.set(v);
  bridge.H4Pwm.set(v);
  bridge.H5Pwm.set(v);
  bridge.H6Pwm.set(v);
  bridge.L1Pwm.set(v);
  bridge.L2Pwm.set(v);
  bridge.L3Pwm.set(v);
  bridge.L4Pwm.set(v);
  bridge.L5Pwm.set(v);
  bridge.L6Pwm.set(v);
  RTWait(10000);
  update();
  v++;
}
