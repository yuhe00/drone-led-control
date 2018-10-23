#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 3*30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 100

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

int gHue;

void loop()
{
  EVERY_N_MILLISECONDS( 20 ) { gHue += 10; } // slowly cycle the "base color" through the rainbow
  fill_rainbow(leds, NUM_LEDS, gHue, 5);
  FastLED.show();
}
