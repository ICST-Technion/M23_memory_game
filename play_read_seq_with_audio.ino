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

#include <HardwareSerial.h>

#define MP3_SERIAL_RX 16  // Connect to MP3 board TX
#define MP3_SERIAL_TX 17  // Connect to MP3 board RX

HardwareSerial mp3Serial(2); // Use HardwareSerial 2 for ESP32
const int buttonPins[NUM_LEDS] = {14, 27, 12, 13, 32};


//global variables for buttons
bool currentButtonStates [] = {true, true,true,true,true}; 
bool previousButtonStates [] = {true, true,true,true,true}; //
bool previousButtonState = HIGH; // will store last time button was updated
unsigned int buttonPress_counters [] = { 0 , 0 ,0,0,0}; //number of presses
unsigned long buttonPress_last_times [] = { 0 , 0 ,0,0,0};   // last time button was pressed
unsigned long last_press_duration= 0; //to allow accessing last press duration as global
int lastSignalTime = 0;
int currentIndex = 0;
int readIndex = 0;
int interval = 500;
byte sequence[10] = {3,2,1,1,4};
bool showing = true;
bool isLighting = true;
void setup() {
  Serial.begin(115200);  // Begin the primary serial for debugging
  mp3Serial.begin(9600, SERIAL_8N1, MP3_SERIAL_RX, MP3_SERIAL_TX); // Begin the secondary serial to communicate with MP3 boa
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
  if(showing)
    playSequence();
    
  else
  {
    readSequence();
  }

    
}


void playSequence()
{
  if(millis() > lastSignalTime + interval)
  {
    if(isLighting){
      lightLED(sequence[currentIndex]);
      playSongInFolder01(sequence[currentIndex]);
    currentIndex +=1;
    lastSignalTime = millis();
    isLighting = false;
    }
    else
    {
      lastSignalTime = millis();
      isLighting = true;
      clearLed();
      if(currentIndex >= 4)
    {
      showing = false;
      currentIndex = 0;
    }
    }
    
  }
}
void gameOver(){
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255,0 ));
  strip.setPixelColor(1, strip.Color(0, 255,0 ));
  strip.setPixelColor(2, strip.Color(0, 255,0 ));
  strip.setPixelColor(3, strip.Color(0, 255,0 ));
  strip.setPixelColor(4, strip.Color(0, 255,0 ));
  strip.show();
  delay(2000);
}

void won(){
  strip.clear();
  strip.setPixelColor(0, strip.Color(255,0,0 ));
  strip.setPixelColor(1, strip.Color(255,0,0 ));
  strip.setPixelColor(2, strip.Color(255,0,0 ));
  strip.setPixelColor(3, strip.Color(255,0,0 ));
  strip.setPixelColor(4, strip.Color(255,0,0 ));
  strip.show();
  delay(2000);
}

void lightLEDPressed(int ledNum)
{
  strip.clear();
  strip.setPixelColor(ledNum, strip.Color(5, 200,160 ));
  strip.show();
}


void readSequence()
{
  for(int button = 0; button<4 ; button++)
  {
    bool res = handle_button(button);
    if(res)
    {
      lightLEDPressed(button+1);
      if(sequence[readIndex] != button + 1)
      {
        gameOver();
        showing=true;
        readIndex=0;
      }
      else
      {
        playSongInFolder01(sequence[readIndex]);
        readIndex ++;
        if(readIndex >= 4){
          won();
          showing=true;
          readIndex=0;
        }
      }
    }

  }
}

void playSignal(byte index)
{

}

void clearLed()
{
  strip.clear();
  strip.show();
}

void lightLED(int ledNum) {
  strip.clear();
  strip.setPixelColor(ledNum, strip.Color(0, 0,255 ));
  strip.show();
}

void mistake() {
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();
}

void success() {
  strip.setPixelColor(0, strip.Color(255, 0, 0));
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
void playSongInFolder01(int songNumber) {
  byte command[] = {0x7E, 0x04, 0x42, 0x01, static_cast<byte>(songNumber), 0xEF};
  mp3Serial.write(command, sizeof(command));  // Send the command to the MP3 board
}
