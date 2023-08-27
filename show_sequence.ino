#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define MATRIX_DATA_PIN 23
#define MATRIX_CLK_PIN 21
#define MATRIX_CS_PIN 22
#define MIN_PRESS_TIME 40
MD_Parola parola = MD_Parola(HARDWARE_TYPE, MATRIX_CS_PIN, MAX_DEVICES);
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, MATRIX_DATA_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, MAX_DEVICES);

#define STRIP_PIN 15
#define NUM_LEDS 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

const int buttonPins[NUM_LEDS] = {14, 27, 12, 13, 32};


//global variables for buttons
bool currentButtonStates [] = {true, true,true,true,true}; 
bool previousButtonStates [] = {true, true,true,true,true}; //
bool previousButtonState = HIGH; // will store last time button was updated
unsigned int buttonPress_counters [] = { 0 , 0 ,0,0,0}; //number of presses
unsigned long buttonPress_last_times [] = { 0 , 0 ,0,0,0};   // last time button was pressed
unsigned long last_press_duration= 0; //to allow accessing last press duration as global
int lastSignalTime = 0;
int currentSequence = 0;
int interval = 500;
byte sequence[10] = {1,0,4,0,3,0,2,0,3,0};
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

void loop() 
{
  if(millis() > lastSignalTime + interval)
  {
    lightLED(sequence[currentSequence]);
    currentSequence +=1;
    if(currentSequence >= 10)
      currentSequence = 0;
    lastSignalTime = millis();
  }
    
}

void playSequence()
{
  
}

void playSignal(byte index)
{

}

void lightLED(int ledNum) {
  strip.clear();
  if(ledNum!=0)
  strip.setPixelColor(ledNum, strip.Color(0, 0, 255));
  strip.show();
}

void mistake() {
  strip.clear();
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
}

void success(int ledNum) {
  strip.clear();
  if(ledNum!=0)
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

bool handle_button(byte button_index) { //handle a button
  bool previousButtonState = previousButtonStates[button_index];
  bool currentButtonState = digitalRead(buttonPins[button_index]);  // check button state's change

  if (currentButtonState != previousButtonState)  //if change is detected
  {
    previousButtonStates[button_index] = currentButtonState;       // save the last state of button
    if (currentButtonState == LOW && previousButtonState == HIGH)  //type of change - from released to pressed
    {
      buttonPress_last_times[button_index] = millis();
    } else {
      last_press_duration = millis() - buttonPress_last_times[button_index];
      if (last_press_duration > MIN_PRESS_TIME) {
        buttonPress_counters[button_index] = buttonPress_counters[button_index] + 1;
        return (true); 
      }
    }
  }
  return (false);
}

