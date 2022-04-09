
#include "config.h"

#include <RTNTPClient.h>
#include <RTWifi.h>
#include <RTMqtt.h>

#include <OpenRealTime.h>

#define PIN_BUTTON 0
#define PIN_PWM 14
RTWifi wifi(WIFI_SSID, WIFI_PASSWORD);
RTMqtt mqtt(wifi, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

RTButton button(PIN_BUTTON, HIGH);

RTNTPClient ntp(NTP_SERVER);

void setup() {
  delay(10);
  Serial.begin(115200);
  delay(10);
  Serial.println("setup");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  wifi.start();
  ntp.setOffset(2*60*60);
  ntp.start();
  mqtt.start();
  
  mqtt.publish("esp/button", "0");
  mqtt.publish("esp/led", "0");
  mqtt.subscribe("esp/led", manageLedMqtt);
  mqtt.subscribe("esp/offset", manageOffsetMqtt);
  
  analogWrite(PIN_PWM, 100);
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("setup done");
}

void loop() {
  button.run();
  wifi.run();
  mqtt.run();
  ntp.run();
  
  manageTime();
  manageButton();
}

void manageTime()
{
  if(ntp.changed())
  {
    //Serial.println(ntp.toChar());
    Serial.print(String(ntp.dayOfMonth()));
    Serial.print("/");
    Serial.print(String(ntp.month()));
    Serial.print("/");
    Serial.print(String(ntp.year()));
    Serial.print("   ");
    Serial.print(String(ntp.hour()));
    Serial.print("h ");
    Serial.print(String(ntp.minute()));
    Serial.print("m ");
    Serial.print(String(ntp.second()));
    Serial.println("s");
  }
}
void manageButton()
{
  static int buttonCount = 0;
  if(button.isTrigged())
  {
    buttonCount++;
    mqtt.publish("esp/button", String(buttonCount));
  }
}

void manageLedMqtt(String message)
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
void manageOffsetMqtt(String message)
{
  ntp.setOffset(message.toInt()*60);
}
