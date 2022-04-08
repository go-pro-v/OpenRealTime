#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Timezone.h>

#include "time.h"

#include <OpenRealTime.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* mqttServer = "YOUR_MQTT_SERVER";
const int mqttPort = 0000; //YOUR_MQTT_PORT
const char* mqttUser = "YOUR_MQTT_USER";
const char* mqttPassword = "YOUR_MQTT_PASSWORD";

const long utcOffsetInSeconds = 0;
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, mqttServer, utcOffsetInSeconds);

#define PIN_BUTTON 0
#define PIN_PWM 14

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool ledState = HIGH;
RTButton button(PIN_BUTTON, HIGH);
RTTimer one_s_timer(1000000);

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

void setup() {
  delay(10);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  ledState = HIGH;
  digitalWrite(LED_BUILTIN, ledState);
  setup_wifi();

  setup_mqtt();
  ledState = LOW;
  digitalWrite(LED_BUILTIN, ledState);
  //Serial.println("Mise en veille prolongée de l'ESP !");
  //ESP.deepSleep(0);
  mqttClient.publish("esp/button", "0");
  mqttClient.publish("esp/led", "0");

  
  configTime(utcOffsetInSeconds, 0, mqttServer);
  //timeClient.begin();
  //timeClient.setTimeOffset(utcOffsetInSeconds);
  analogWrite(PIN_PWM, 100);
}

void loop() {
  //timeClient.update();
  button.run();
  if(one_s_timer.isOver())
  {
    one_s_timer.sync_reset();
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    rawtime = CE.toLocal(rawtime);
    timeinfo = localtime (&rawtime);
    Serial.println(asctime(timeinfo));
    /*Serial.print(timeClient.getHours());
    Serial.print(":");
    Serial.print(timeClient.getMinutes());
    Serial.print(":");
    Serial.println(timeClient.getSeconds());*/
  }
// blink led
  /*static auto ledTimer = millis();

  if(ledTimer + 500 < millis())
  {
    ledTimer += 500;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    //Serial.println("Blink");
  }*/

// utilisation du bouton
  static int buttonCount = 0;
  if(button.isTrigged())
  {
    buttonCount++;
    mqttClient.publish("esp/button", String(buttonCount).c_str());
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    wifi_reconnect();
  }
  if (!mqttClient.connected())
  {
    mqtt_reconnect();
  }
  mqttClient.loop();



}

void setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println();
    Serial.print("Connection au réseau : ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connecté");
    Serial.print("Addresse IP : ");
    Serial.println(WiFi.localIP());
  }
  randomSeed(micros());

}

void wifi_reconnect()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.print("Reconnection au réseau : ");
    Serial.println(ssid);
    WiFi.disconnect();
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connecté");
    Serial.print("Addresse IP : ");
    Serial.println(WiFi.localIP());
  }
}

void setup_mqtt()
{
  mqttClient.setServer(mqttServer, mqttPort); // change port number as mentioned in your cloudmqtt console
  mqttClient.setCallback(MQTTCallback);
  mqtt_reconnect();
  
}
void mqtt_reconnect()
{
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword ))
    {
      Serial.println("connected");
      mqttClient.subscribe("esp/led");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void MQTTCallback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  String message;
  
  for (int i = 0; i < length; i++) {
    message = String(message + (char)payload[i]);
  }
  message = String(message + '\0');
  Serial.print("Message:");
  Serial.print(message);
  Serial.println("");
  
  if (strcmp(topic, "esp/led") == 0)
  { 
    if (message == "1")
    {
      digitalWrite(LED_BUILTIN, LOW); 
    }
    if (message == "0")
    {
      digitalWrite(LED_BUILTIN, HIGH); 
    }
  }

  Serial.print("Message:");
  Serial.println("");
}
