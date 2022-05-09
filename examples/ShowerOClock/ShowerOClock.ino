
#include "config.h"
#define FAST_LED_MODE
//#define NEOPIXEL_BUS_MODE
//#define NEOPIXEL_ADAFRUIT_MODE

#include <RTNTPClient.h>
#include <RTWifi.h>
#include <RTMqtt.h>
#include <RTRamp.h>

#include <OpenRealTime.h>
#include <TM1637Display.h>

#if defined(FAST_LED_MODE)
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#elif defined(NEOPIXEL_BUS_MODE)
#include <NeoPixelBrightnessBus.h>
#elif defined(NEOPIXEL_ADAFRUIT_MODE)
#include <Adafruit_NeoPixel.h>
//#include <avr/power.h>
#endif
//24

#if defined(FAST_LED_MODE)
CRGB Leds[NEOPIXEL_NUM];
#elif defined(NEOPIXEL_BUS_MODE)
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> neo_pixels(NEOPIXEL_NUM, NEOPIXEL_PIN);
#elif defined(NEOPIXEL_ADAFRUIT_MODE)
Adafruit_NeoPixel neo_pixels(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif


RTWifi wifi(WIFI_SSID, WIFI_PASSWORD);
RTMqtt mqtt(wifi, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

RTButton button(PIN_BUTTON, HIGH, 100);

RTNTPClient ntp(NTP_SERVER);

RTRamp rampShower(100, 255);
RTRamp rampLight(1000, 255);

TM1637Display display_7_4(PIN_I2C_CLK, PIN_I2C_DIO);

bool isRingAllowed = false;
int ringHour = 0;
int ringMinute = 0;
bool isRinging = false;
bool isRingTest = false;
bool ledState = LOW;

uint8_t led_R = 0;
uint8_t led_G = 0;
uint8_t led_B = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  analogWriteFreq(100);
  analogWrite(PIN_PWM, 0);
  digitalWrite(LED_BUILTIN, LOW);
  
  delay(10);
  Serial.begin(115200);
  delay(10);
  Serial.println("setup");
  display_7_4.setBrightness(7);
  
#if defined(FAST_LED_MODE)
  FastLED.addLeds<WS2812B, NEOPIXEL_PIN, GRB>(Leds, NEOPIXEL_NUM).setCorrection(TypicalLEDStrip);;
  //FastLED.delay(100);
  FastLED.setBrightness(0);
  FastLED.setMaxRefreshRate(100);
#elif defined(NEOPIXEL_BUS_MODE)
  neo_pixels.Begin();
  neo_pixels.SetBrightness(0);
  neo_pixels.Show(); 
#elif defined(NEOPIXEL_ADAFRUIT_MODE)
  neo_pixels.begin(); 
  //delay(500);
  neo_pixels.setBrightness(0);
  
  neo_pixels.clear(); 
  neo_pixels.show(); 
#endif
  updateLeds();
  
  wifi.start(
    [](){
      static RTTimer stepTimer(500000);
      if(stepTimer.isOver())
      {
        stepTimer.sync_reset();
        stepChenille();
      }
    }
  );
  ntp.setOffset(2*60*60);
  ntp.start();
  
  rampShower.start();
  rampShower.set(0);
  rampLight.start();
  rampLight.set(0);

  mqtt.start();
  mqtt.subscribe("clock/ring", [](String message){isRingAllowed = (message == "1");});
  mqtt.subscribe("clock/test/ring", [](String message){isRingTest = (message == "1");});
  mqtt.subscribe("clock/hour", [](String message){ringHour = message.toInt();});
  mqtt.subscribe("clock/minute", [](String message){ringMinute = message.toInt();});
  mqtt.subscribe("clock/led", manageLedMqtt);
  mqtt.subscribe("clock/offset", [](String message){ntp.setOffset(message.toInt()*60);});
  mqtt.subscribe("clock/led/r", [](String message){led_R = message.toInt();updateLeds();});
  mqtt.subscribe("clock/led/g", [](String message){led_G = message.toInt();updateLeds();});
  mqtt.subscribe("clock/led/b", [](String message){led_B = message.toInt();updateLeds();});
  mqtt.subscribe("clock/brightness", manageClockBrightness);
  
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("setup done");
}

void loop() {
  button.run();
  wifi.run();
  mqtt.run();
  ntp.run();
  rampShower.run();
  rampLight.run();
  
  manageDilplayTime();
  manageRinging();

  if(isRinging)
  {
    rampShower.set(255);
  }
  else
  {
    rampShower.set(0);
  }
  
  if(rampShower.changed())
  {
    analogWrite(PIN_PWM, rampShower.get());
  }
  
  if(button.isTrigged())
  {
    if(digitalRead(LED_BUILTIN))
    {
      mqtt.publish("clock/led", "1");
      ledState = HIGH;
      Serial.println("button on");
      updateLeds();
    }
    else
    {
      mqtt.publish("clock/led", "0");
      ledState = LOW;
      Serial.println("button off");
      updateLeds();
    }
  }
  digitalWrite(LED_BUILTIN, !ledState);

  if(ledState)
  {
    rampLight.set(255);
  }
  else
  {
    rampLight.set(0);
  }
  
  static RTTimer ledTimer(40000);
  if(ledTimer.isOver())
  {
    ledTimer.async_reset();
#if defined(FAST_LED_MODE)
    FastLED.setBrightness(rampLight.get());
    //noInterrupts();
    FastLED.show();
    //interrupts();
#elif defined(NEOPIXEL_BUS_MODE)
    neo_pixels.SetBrightness(rampLight.get());
    neo_pixels.Show();
#elif defined(NEOPIXEL_ADAFRUIT_MODE)
    noInterrupts();
    neo_pixels.show();
    interrupts();
#endif
  }
}

void updateLeds()
{
  #if defined(FAST_LED_MODE)
  for(int i = 0 ; i<NEOPIXEL_NUM; i++)
  {
    Leds[i] = CRGB(led_R,led_G,led_B);
  }
#elif defined(NEOPIXEL_BUS_MODE)
  RgbColor couleur(led_R, led_G, led_B);
  neo_pixels.ClearTo(RgbColor(led_R, led_G, led_B););
#elif defined(NEOPIXEL_ADAFRUIT_MODE)
  neo_pixels.fill(neo_pixels.Color(led_R,led_G,led_B));
#endif
}
void stepChenille()
{
  const uint8_t digit1[] = {SEG_A,0,0,0, 0,0,         0,0,0,SEG_D, SEG_E,SEG_F};
  const uint8_t digit2[] = {0,SEG_A,0,0, 0,0,         0,0,SEG_D,0, 0,0        };
  const uint8_t digit3[] = {0,0,SEG_A,0, 0,0,         0,SEG_D,0,0, 0,0        };
  const uint8_t digit4[] = {0,0,0,SEG_A, SEG_B,SEG_C, SEG_D,0,0,0, 0,0        };

  static int step = -1;

  step = (step + 1) % 12;
  
  uint8_t data[] = {
    digit1[step], 
    digit2[step], 
    digit3[step], 
    digit4[step]
  };
  display_7_4.setSegments(data);
}

bool displayAllowed = true;
void manageClockBrightness(String message)
{
  uint8_t brightness = message.toInt();
  if(brightness == 0)
  {
    displayAllowed = false;
    uint8_t data[] = {0,0,0,0};
    display_7_4.setSegments(data);
  }
  else
  {
    displayAllowed = true;
    display_7_4.setBrightness(brightness - 1);
  }
}

void manageDilplayTime()
{
  if(displayAllowed && ntp.changed())
  {
    static uint8_t dot = 0;
    dot = ~dot;
    display_7_4.showNumberDecEx(
      ntp.hour() * 100 + ntp.minute(), 
      dot, true, 4, 0);
  }
}

void manageLedMqtt(String message)
{
  if (message == "1")
  {
    ledState = HIGH;
  }
  if (message == "0")
  {
    ledState = LOW; 
  }
  updateLeds();
}

void manageRinging()
{
  if(isRinging && button.isTrigged())
  {
    isRinging = false;
    isRingTest = false;
    Serial.println("Stop sonne");
    if(isRingTest)
    {
      mqtt.publish("clock/test/ring", "0");
    }
  }
  
  if(isRingTest)
  {
    isRinging = true;
  }
  else if(isRingAllowed)
  {
    if(ntp.changed())
    {
      if(!isRinging && ntp.hour() == ringHour && ntp.minute() == ringMinute && ntp.second() == 0)
      {
        Serial.println("Sonne !");
        isRinging = true;
      }
      else if (isRinging)
      {
        if(ntp.hour() != ringHour || ntp.minute() != ringMinute || ntp.second() >= 30)
        {
          isRinging = false;
          Serial.println("Stop sonne");
        }
      }
    }
  }
  else if(isRinging)
  {
    isRinging = false;
    Serial.println("Stop sonne");
  }
}
