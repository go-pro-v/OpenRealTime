#ifndef RTWIFI
#define RTWIFI
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <RTTimer.h>

class RTWifi
{
public:
	RTWifi(String ssid, String password);
	void run();
	void start();
	WiFiClient& getWiFiClient();

private:
	WiFiClient wifiClient;
	void checkWifiReconnection();
	bool reconnecting = false;
	RTTimer reconnectTimer = RTTimer(500000);
	String  wifi_ssid;
	String  wifi_password;
};

#endif