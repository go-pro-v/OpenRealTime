#ifndef RTMQTT
#define RTMQTT
#include <Arduino.h>
#include <RTWifi.h>
#include <RTTimer.h>
#include <PubSubClient.h>

class RTMqtt
{
public:
	RTMqtt(RTWifi& wifiClient, String server, int port, String user, String password);
	void start();
	void run();
	void publish(String topic, String value);
	void subscribe(String topic, void (*callback)(String) = [](String){});

private:
	PubSubClient mqttClient;
	String mqtt_server;
	int mqtt_port;
	String mqtt_user;
	String mqtt_password;
	RTTimer reconnectTimer = RTTimer(30*1000*1000);
	bool reconnecting = false;
	RTWifi* wifi_client;

	class Topic{
	public:
		Topic(String topic, Topic* topic_list = 0, void (*callback)(String) = [](String){});
		String mqtt_topic;
		Topic* next = 0;
		void (*mqtt_callback)(String);
	};
	static Topic* topic_list;

	void mqtt_reconnect();
	static void MQTTCallback(char* topic, byte* payload, unsigned int length);
};

#endif