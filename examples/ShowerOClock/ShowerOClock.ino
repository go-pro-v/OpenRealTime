
#include "config.h"

#include <RTNTPClient.h>
#include <RTWifi.h>
#include <RTMqtt.h>
#include <RTRamp.h>

#include <OpenRealTime.h>

#define PIN_BUTTON 0
#define PIN_PWM 14
RTWifi wifi(WIFI_SSID, WIFI_PASSWORD);
RTMqtt mqtt(wifi, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

RTButton button(PIN_BUTTON, HIGH);

RTNTPClient ntp(NTP_SERVER);

RTRamp ramp(1000, 255);

void setup() {
  delay(10);
  Serial.begin(115200);
  delay(10);
  Serial.println("setup");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  analogWriteFreq(100);
  analogWrite(PIN_PWM, 0);
  digitalWrite(LED_BUILTIN, LOW);
  
  wifi.start();
  ntp.setOffset(2*60*60);
  ntp.start();
  mqtt.start();
  ramp.start();
  
  mqtt.publish("esp/button", "0");
  mqtt.publish("esp/led", "0");
  mqtt.subscribe("esp/led", manageLedMqtt);
  mqtt.subscribe("esp/offset", manageOffsetMqtt);
  
  ramp.set(255);
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("setup done");
}

void loop() {
  button.run();
  wifi.run();
  mqtt.run();
  ntp.run();
  ramp.run();
  
  manageTime();
  manageButton();

  static RTTimer ledTimer(2000000);
  
  if(ramp.changed())
  {
    //Serial.println(String(ramp.get()));
    analogWrite(PIN_PWM, ramp.get());
    analogWrite(LED_BUILTIN, ramp.get());
    if(ramp.get() == 255)
    {
      //analogWrite(PIN_PWM, 25);
      //ramp.set(0);
      ledTimer.async_reset();
      Serial.println("255");
    }
    if(ramp.get() == 0)
    {
      //analogWrite(PIN_PWM, 0);
      //ramp.set(255);
      ledTimer.async_reset();
      Serial.println("0");
    }
  }
  if(ledTimer.isOver() && ramp.get() == 0)
  {
      Serial.println("target 255");
      ramp.set(255);
      ledTimer.async_reset();
  }
  if(ledTimer.isOver() && ramp.get() == 255)
  {
      Serial.println("target 0");
      ramp.set(0);
      ledTimer.async_reset();
  }
  
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
