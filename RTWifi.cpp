#include <RTWifi.h>

RTWifi::RTWifi(String ssid, String password)
{
	wifi_ssid = ssid;
	wifi_password = password;
}
void RTWifi::start(void (*duringConnection)(),void (*duringReconnection)())
{
	wifi_during_reconnection = duringReconnection;
	if (WiFi.status() != WL_CONNECTED) 
	{
		WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
		Serial.println();
		Serial.print("Connection au réseau : ");
		Serial.println(wifi_ssid.c_str());
		while (WiFi.status() != WL_CONNECTED) 
		{
			yield();
			duringConnection();
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
	if(!reconnecting && WiFi.status() != WL_CONNECTED) {
		reconnecting = true;
		Serial.println();
		Serial.print("Reconnection au réseau : ");
		Serial.println(wifi_ssid.c_str());
		WiFi.disconnect();
		WiFi.reconnect();
	}
	else if(reconnecting && WiFi.status() != WL_CONNECTED)
	{
		if(reconnectTimer.isOver())
		{
			reconnectTimer.sync_reset();
			Serial.print(".");
		}
	}
	else if(reconnecting && WiFi.status() == WL_CONNECTED)
	{
		reconnecting = false;
		Serial.println("");
		Serial.println("WiFi connecté");
		Serial.print("Addresse IP : ");
		Serial.println(WiFi.localIP());
	}
}