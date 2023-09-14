#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 5
#define LED_PIN 2
#define BUTTON_COUNT 5

const int buttonPins[BUTTON_COUNT] = {22, 3, 21, 1, 19};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(buttonPins[i]) == LOW) { // Button pressed
      lightLED(i);
    }
  }
}

void lightLED(int ledNum) {
  strip.clear(); // Turn off all LEDs
  strip.setPixelColor(ledNum, strip.Color(255, 0, 0));  // Light up the desired LED in red
  strip.show();
}
