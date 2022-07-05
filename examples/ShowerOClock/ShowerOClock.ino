
#include "config.h"
#define FAST_LED_MODE
//#define NEOPIXEL_BUS_MODE
//#define NEOPIXEL_ADAFRUIT_MODE
#define RING_DURATION 15

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

RTRamp rampShower(200, PWMRANGE);
RTRamp rampLight(1000, 255);

TM1637Display display_7_4(PIN_I2C_CLK, PIN_I2C_DIO);

enum class RingState{
  waiting,
  ringing,
  stopped,
  disabled,
  ringtest
};
/*bool isRingAllowed = false;
bool isRinging = false;
bool isRingTest = false;*/
RingState ring_state = RingState::waiting;
int ringHour = 0;
int ringMinute = 0;
bool ledState = LOW;
bool visualMMI = false;
bool visualMMI2 = false;

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
      stepTimer.run();
      if(stepTimer.isTrigged())
      {
        stepChenille();
      }
    },
    [](){visualMMI2 = true;}
  );
  //ntp.setOffset(2*60*60);
  ntp.start();
  
  rampShower.start();
  rampShower.set(0);
  rampLight.start();
  rampLight.set(0);

  mqtt.start();
  mqtt.subscribe("clock/ring", [](String message){message == "1" ? enableRinging() : disableRinging(); visualMMI = true;});
  mqtt.subscribe("clock/test/ring", [](String message){message == "1" ? startRingtest() : stopRingtest(); visualMMI = true;});
  mqtt.subscribe("clock/hour", [](String message){ringHour = message.toInt(); visualMMI = true;});
  mqtt.subscribe("clock/minute", [](String message){ringMinute = message.toInt(); visualMMI = true;});
  mqtt.subscribe("clock/led", manageLedMqtt);
  mqtt.subscribe("clock/offset", [](String message){ntp.setOffset(message.toInt()*60); visualMMI = true;});
  mqtt.subscribe("clock/led/r", [](String message){led_R = message.toInt();updateLeds(); visualMMI = true;});
  mqtt.subscribe("clock/led/g", [](String message){led_G = message.toInt();updateLeds(); visualMMI = true;});
  mqtt.subscribe("clock/led/b", [](String message){led_B = message.toInt();updateLeds(); visualMMI = true;});
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

  switch(ring_state)
  {
    case RingState::ringing:
    case RingState::ringtest:
      rampShower.set(PWMRANGE);
      break;
    default:
      rampShower.set(0);
      break;
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
  ledTimer.run();
  if(ledTimer.isTrigged())
  {
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
    display_7_4.setBrightness(0);
    uint8_t data[] = {0,0,0,0};
    display_7_4.setSegments(data);
  }
  else
  {
    displayAllowed = true;
    display_7_4.setBrightness(brightness - 1);
  }
   visualMMI = true;
}

void manageDilplayTime()
{
  static RTTimer visualMMITimer(500000);
  static bool isGlitching = false;
  visualMMITimer.update();
  if(visualMMI || visualMMI2)
  {
    isGlitching = true;
    visualMMITimer.async_reset();
    uint8_t data[4] = {0,0,0,0};
    if(visualMMI)
    {
      data[0] = SEG_A|SEG_D|SEG_E|SEG_F;
      data[1] = SEG_A|SEG_D;
      data[2] = SEG_A|SEG_D; 
      data[3] = SEG_A|SEG_B|SEG_C|SEG_D;
    }
    else if(visualMMI2)
    {
      data[0] = SEG_B|SEG_C|SEG_E|SEG_F;
      data[1] = SEG_B|SEG_C|SEG_E|SEG_F;
      data[2] = SEG_B|SEG_C|SEG_E|SEG_F;
      data[3] = SEG_B|SEG_C|SEG_E|SEG_F;
    }
    display_7_4.setSegments(data);
    visualMMI = false;
    visualMMI2 = false;
  }
  if(isGlitching)
  {
    if(visualMMITimer.isOver())
    {
      isGlitching = false;
      if(displayAllowed)
      {
        display_7_4.showNumberDecEx(
          ntp.hour() * 100 + ntp.minute(), 
          true, true, 4, 0);
      }
      else
      {
        uint8_t data[] = {0,0,0,0};
        display_7_4.setSegments(data);
      }
    }
  }
  else if(displayAllowed && ntp.changed())
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
  visualMMI = true;
}
  
void disableRinging()
{
  if(ring_state != RingState::disabled)
  {
    Serial.println("Disable ring");
    ring_state = RingState::disabled;
  }
}
void enableRinging()
{
  if(ring_state == RingState::disabled)
  {
    Serial.println("Enable ring");
    ring_state = RingState::waiting;
  }
}
void startRingtest()
{
  if(ring_state != RingState::disabled && ring_state != RingState::ringtest)
  {
    Serial.println("Test ring");
    ring_state = RingState::ringtest;
  }
}
void stopRingtest()
{
  if(ring_state == RingState::ringtest)
  {
    Serial.println("Stop test ring");
    ring_state = RingState::waiting;
  }
}
void manageRinging()
{
  if(button.isTrigged())
  {
    if(ring_state==RingState::ringtest)
    {
      mqtt.publish("clock/test/ring", "0");
    }
    else if(ring_state==RingState::ringing)
    {
      ring_state=RingState::stopped;
      Serial.println("Stop ring");
    }
  }
  if(ring_state==RingState::waiting && ntp.changed() && ntp.hour() == ringHour && ntp.minute() == ringMinute && ntp.second() < RING_DURATION)
  {
    ring_state=RingState::ringing;
    Serial.println("Ring");
  }
  if(
    (ring_state==RingState::stopped || ring_state==RingState::ringing)
    && ntp.changed() 
    && (ntp.hour() != ringHour || ntp.minute() != ringMinute || ntp.second() >= RING_DURATION)
  )
  {
    ring_state=RingState::waiting;
    Serial.println("Reinit ring");
  }
}
