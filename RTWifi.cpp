#include <RTWifi.h>

RTWifi::RTWifi(String ssid, String password)
{
	wifi_ssid = ssid;
	wifi_password = password;
}
bool RTWifi::connected()
{
	return WiFi.status() == WL_CONNECTED;
}
void RTWifi::start(void (*duringConnection)(),void (*duringReconnection)())
{
	wifi_during_reconnection = duringReconnection;
	if (!connected()) 
	{
		WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
		Serial.println();
		Serial.print("Connection au réseau : ");
		Serial.println(wifi_ssid.c_str());
		reconnectTimer.update();
		reconnectTimer.async_reset();
		while (!connected()) 
		{
			yield();
			duringConnection();
			reconnectTimer.update();
			if(reconnectTimer.isOver())
			{
				reconnectTimer.sync_reset();
				Serial.print(".");
			}
		}
		Serial.println("");
		Serial.println("WiFi connecté");
		Serial.print("Addresse IP : ");
		Serial.println(WiFi.localIP());
	}
	randomSeed(micros());
}
void RTWifi::run()
{
	checkWifiReconnection();
}
WiFiClient& RTWifi::getWiFiClient()
{
	return wifiClient;
}
void RTWifi::checkWifiReconnection()
{
	if(!reconnecting && !connected()) {
		reconnectTimer.update();
		reconnectTimer.async_reset();
		wifi_during_reconnection();
		reconnecting = true;
		Serial.println();
		Serial.print("Reconnection au réseau : ");
		Serial.println(wifi_ssid.c_str());
		WiFi.disconnect();
		WiFi.reconnect();
	}
	else if(reconnecting && !connected())
	{
		wifi_during_reconnection();
		reconnectTimer.update();
		if(reconnectTimer.isOver())
		{
			reconnectTimer.sync_reset();
			Serial.print(".");
		}
	}
	else if(reconnecting && connected())
	{
		reconnecting = false;
		Serial.println("");
		Serial.println("WiFi connecté");
		Serial.print("Addresse IP : ");
		Serial.println(WiFi.localIP());
	}
}