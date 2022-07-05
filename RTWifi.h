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
	void start(void (*duringConnection)() = [](){}, void (*duringReconnection)() = [](){});
	WiFiClient& getWiFiClient();
	bool connected();

private:
	WiFiClient wifiClient;
	void checkWifiReconnection();
	bool reconnecting = false;
	RTTimer reconnectTimer = RTTimer(500000);
	String  wifi_ssid;
	String  wifi_password;
	void (*wifi_during_reconnection)();
};

#endif