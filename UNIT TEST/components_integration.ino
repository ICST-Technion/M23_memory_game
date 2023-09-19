#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define MATRIX_DATA_PIN 23
#define MATRIX_CLK_PIN 21
#define MATRIX_CS_PIN 22

MD_Parola parola = MD_Parola(HARDWARE_TYPE, MATRIX_CS_PIN, MAX_DEVICES);
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, MATRIX_DATA_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, MAX_DEVICES);

#define STRIP_PIN 15
#define NUM_LEDS 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

const int buttonPins[NUM_LEDS] = {14, 27, 12, 13, 32};
bool buttonStates[NUM_LEDS] = {false, false, false, false, false};

void setup() {
  strip.begin();
  strip.show();
  
  parola.begin();
  parola.displayClear();
  parola.setIntensity(0);
  matrix.begin();

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (!digitalRead(buttonPins[i]) && !buttonStates[i]) {  // Button is pressed and was not pressed before
      buttonStates[i] = true;
      lightLED(i);
      lightMatrixRow(i);
      delay(50);  // Debounce delay
    } else if (digitalRead(buttonPins[i]) && buttonStates[i]) {  // Button is released and was pressed before
      buttonStates[i] = false;
      delay(50);  // Debounce delay
    }
  }
}

void lightLED(int ledNum) {
  strip.clear();
  strip.setPixelColor(ledNum, strip.Color(255, 0, 0));
  strip.show();
}

void lightMatrixRow(int rowNum) {
  matrix.clear(rowNum);
  for (int i = 0; i < 8; i++) {
    matrix.setPoint(rowNum, i, true);
  }
  matrix.update();
}
