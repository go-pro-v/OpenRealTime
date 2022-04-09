#ifndef RTNTPCLIENT
#define RTNTPCLIENT
#include <Arduino.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <TimeLib.h>

#ifdef dayOfWeek
#undef dayOfWeek
#endif

class RTNTPClient
{
public:
	RTNTPClient(String ntp_server);
	int second();
	int minute();
	int hour();
	int dayOfMonth();
	int month();
	int year();
	int dayOfWeek();
	int dayOfYear();
	char* toChar();
	void run();
	bool changed();
	void start();
	void setOffset(int offset);
private:
	String server_name;
	struct tm * timeinfo = 0;
	int previousSecond = -1;
	int currentSecond = -1;
	WiFiUDP ntpUDP;
	NTPClient timeClient = NTPClient(ntpUDP);
};

#endif