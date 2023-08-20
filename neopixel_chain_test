#include <Adafruit_NeoPixel.h>

#define PIN 15
#define BUTTON1_PIN 14
#define BUTTON2_PIN 4
#define BUTTON3_PIN 5
#define BUTTON4_PIN 12
#define BUTTON5_PIN 13

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  strip.begin();

  // Clear out any state upon initialization
  for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0); // Set each pixel to 'off'
  }
  strip.show();

  strip.setBrightness(50);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);
}

void loop() {
    if (!digitalRead(BUTTON1_PIN)) {
        strip.setPixelColor(0, strip.Color(255, 0, 0)); // Set first LED to red
        strip.show();
    } else {

        strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn off first LED
        strip.show();
    }

    if (!digitalRead(BUTTON2_PIN)) {
        strip.setPixelColor(1, strip.Color(0, 255, 0)); // Set second LED to green
        strip.show();
    } else {
        strip.setPixelColor(1, strip.Color(0, 0, 0)); // Turn off second LED
        strip.show();
    }

    if (!digitalRead(BUTTON3_PIN)) {
        strip.setPixelColor(2, strip.Color(0, 0, 255)); // Set third LED to blue
        strip.show();
    } else {
        strip.setPixelColor(2, strip.Color(0, 0, 0)); // Turn off third LED
        strip.show();
    }

    if (!digitalRead(BUTTON4_PIN)) {
        strip.setPixelColor(3, strip.Color(255, 255, 0)); // Set fourth LED to yellow
        strip.show();
    } else {
        strip.setPixelColor(3, strip.Color(0, 0, 0)); // Turn off fourth LED
        strip.show();
    }

    if (!digitalRead(BUTTON5_PIN)) {
        strip.setPixelColor(4, strip.Color(0, 255, 255)); // Set fifth LED to cyan
        strip.show();
    } else {
        strip.setPixelColor(4, strip.Color(0, 0, 0)); // Turn off fifth LED
        strip.show();
    }

    delay(20); // Debounce delay
}
