#include <RTMqtt.h>
RTMqtt::RTMqtt(RTWifi& wifiClient, String server, int port, String user, String password)
{
	mqtt_server = server;
	mqtt_port = port;
	mqtt_user = user;
	mqtt_password = password;
	mqttClient = PubSubClient(wifiClient.getWiFiClient());
}
void RTMqtt::start()
{
	mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
	mqttClient.setCallback(MQTTCallback);
	mqtt_reconnect();
}
void RTMqtt::run()
{
	if (!mqttClient.connected())
	{
		mqtt_reconnect();
	}
	else
	{
		mqttClient.loop();
	}
}
void RTMqtt::publish(String topic, String value)
{
		mqttClient.publish(topic.c_str(), value.c_str());
}
void RTMqtt::subscribe(String topic,void (*callback)(String))
{
	topic_list = new Topic(topic, topic_list, callback);
	if (mqttClient.connected())
	{
		mqttClient.subscribe(topic_list->mqtt_topic.c_str());
	}
}
RTMqtt::Topic::Topic(String topic, Topic* topic_list, void (*callback)(String))
{
	mqtt_topic = topic;
	next = topic_list;
	mqtt_callback = callback;
}

void RTMqtt::mqtt_reconnect()
{
	if(!reconnecting && !mqttClient.connected()) {
		reconnecting = true;
		Serial.println("Connexion à MQTT...");
		if (mqttClient.connect("ESP8266Client", mqtt_user.c_str(), mqtt_password.c_str()))
		{
			Serial.println("Connecté");
			Topic* current_topic = topic_list;
			while(current_topic != 0)
			{
				mqttClient.subscribe(current_topic->mqtt_topic.c_str());
				current_topic = current_topic->next;
			}
		}
		else
		{
			Serial.print("Échèque de connexion à MQTT : ");
			Serial.print(mqttClient.state());
			Serial.println("");
		}
	}
	else if(reconnecting && !mqttClient.connected())
	{
		if(reconnectTimer.isOver())
		{
			reconnecting = false;
		}
	}
}
void RTMqtt::MQTTCallback(char* topic, byte* payload, unsigned int length)
{
	Serial.print("Message reçu : ");
	Serial.print(topic);
	String message;

	for (int i = 0; i < length; i++) {
		message = String(message + (char)payload[i]);
	}
	message = String(message + '\0');
	Serial.print(" : ");
	Serial.print(message);
	Serial.println("");

	Topic* current_topic = topic_list;
	while(current_topic != 0)
	{
		if (strcmp(topic, current_topic->mqtt_topic.c_str()) == 0)
		{
			current_topic->mqtt_callback(message);
		}
		current_topic = current_topic->next;
	}
}