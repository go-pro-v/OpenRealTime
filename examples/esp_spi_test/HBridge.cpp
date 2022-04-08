#include "HBridge.h"

HBridge::HBridge(uint8_t spi_bus, uint8_t slave_pin){
	spi = SPIClass(spi_bus);
	pin = slave_pin;
	init();
}
HBridge::HBridge(SPIClass & spi_bus, uint8_t slave_pin){
	spi = spi_bus;
	pin = slave_pin;
	init();
}
void HBridge::init()
{
	spi.begin();
	pinMode(pin, OUTPUT);
}
HBridge::~HBridge(){}
void HBridge::run(){
	H1Pwm.run();
	H2Pwm.run();
	H3Pwm.run();
	H4Pwm.run();
	H5Pwm.run();
	H6Pwm.run();
	L1Pwm.run();
	L2Pwm.run();
	L3Pwm.run();
	L4Pwm.run();
	L5Pwm.run();
	L6Pwm.run();
	setData();
	spi.beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE1));
	digitalWrite(pin, LOW);
	spi.transfer16(data);
	digitalWrite(pin, HIGH);
	spi.endTransaction();
}
void HBridge::setData()
{
	H1 = H1Pwm.get();
	H2 = H2Pwm.get();
	H3 = H3Pwm.get();
	H4 = H4Pwm.get();
	H5 = H5Pwm.get();
	H6 = H6Pwm.get();
	L1 = L1Pwm.get();
	L2 = L2Pwm.get();
	L3 = L3Pwm.get();
	L4 = L4Pwm.get();
	L5 = L5Pwm.get();
	L6 = L6Pwm.get();
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
uint16_t HBridge::place(bool state, uint8_t position)
{
	return state?(0x0001 << position):0x0000;
}
