#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
//LedMatrix definitions
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define MATRIX_DATA_PIN 23
#define MATRIX_CLK_PIN 21
#define MATRIX_CS_PIN 22
MD_Parola parola = MD_Parola(HARDWARE_TYPE, MATRIX_CS_PIN, MAX_DEVICES);
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, MATRIX_DATA_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, MAX_DEVICES);


//Ledstrip (Adafruit neopixel) definitions
#define STRIP_PIN 15
#define NUM_LEDS 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);


//Mp3 board definitions
#include <HardwareSerial.h>
#define MP3_SERIAL_RX 16  // Connect to MP3 board TX
#define MP3_SERIAL_TX 17  // Connect to MP3 board RX
HardwareSerial mp3Serial(2);


//Bluetooth object
BluetoothSerial SerialBT;
String readBuffer = "";


//Buttons (order should correspond to this array)
const int buttonPins[NUM_LEDS] = {14, 27, 12, 13, 32};
//Setting initial values and recording button states
bool currentButtonStates [] = {true, true,true,true,true}; 
bool previousButtonStates [] = {true, true,true,true,true};
#define MIN_PRESS_TIME 40


//Used as a previous button presses debugging. counts individual button presses
unsigned int buttonPress_counters [] = {0,0,0,0,0}; 
//Used as a record of button presses times
unsigned long buttonPress_last_times [] = {0,0,0,0,0}; 
//to allow accessing last press duration as global
unsigned long last_press_duration= 0; 

//Records previous machine state time to handle fluent transitions
int lastSignalTime = 0;

//Represents current sequence length SHOWING
int currentIndex = 0;

//Represents current sequence length READING
int readIndex = 0;

//Holds current intial game sequence start time
unsigned long startTime;

//Holds the current entered sequence length (desired length of presses by user)
int currentSequenceLength = 1;

//Defining sequence(currently pre-defined)
#define MAX_SEQUENCE_LENGTH 5
byte sequence[MAX_SEQUENCE_LENGTH] = {3,2,1,0,4};


//Alternating bool for short blink effect when displaying sequence
bool isLighting = true;


//Machine states
enum GAME_STATE{
  PLAY,
  READ,
  WON,
  LOST,
  MENU
};
//For obvious reasons game starts at MENU
GAME_STATE currentState  = MENU;

//Additional commands should be added for complex capabilities using BlueTooth connection via mobile device
enum COMMAND{
  START_GAME,
  NO_COMMAND
};


void setup() 
{

  Serial.begin(115200);  // Begin the primary serial for debugging
  mp3Serial.begin(9600, SERIAL_8N1, MP3_SERIAL_RX, MP3_SERIAL_TX); // Begin the secondary serial to communicate with MP3 boa
  strip.begin();
  strip.show();
  SerialBT.begin("ESP32test"); //Bluetooth device name
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

 switch(currentState)
  {
    case MENU:
      //for now check for bluetooth input from mobile device in MENU only
      checkBlueToothInputCommand();
      break;
    case PLAY:
      playSequence();
      break;
    case READ:
      readSequence();
      break;
    case WON:
      won();
      break;
    case LOST:
      gameOver();
      break;
    default:
    break;
  }

}

void checkBlueToothInputCommand()
{


  unsigned long currentTime = millis();  // Correct the type for millis()
  if(currentTime > lastSignalTime + 900)
  {
    while (SerialBT.available()) 
    {  // Read from Bluetooth Serial
      char c = SerialBT.read();
      readBuffer += c;
      Serial.print(c);
    }
    if (readBuffer.length() > 100) {  // Trim the buffer if it gets too long.
      readBuffer = readBuffer.substring(readBuffer.length() - 100);
    }

    if (readBuffer.indexOf("start_game") != -1) {  // If "start_game" is found in the buffer...
      readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
      currentState = PLAY;
      startTime = millis();
    }
  }
}

void playSequence()
{
  if(millis() > lastSignalTime + 500)
  {
      drawSquare();
    if(isLighting){
      lightLED(sequence[currentIndex]);
      playSongInFolder01(sequence[currentIndex]+1);
    currentIndex +=1;
    lastSignalTime = millis();
    isLighting = false;
    }
    else
    {
      lastSignalTime = millis();
      isLighting = true;
      clearLed();
      if(currentIndex >= currentSequenceLength)
    {
      currentState = READ;
      currentIndex = 0;
    }
    }
    
  }
}
void gameOver(){
  currentState = LOST;
  int currentTime = millis();
  if(currentTime > lastSignalTime + 800)
  {
    send_game_time();
    drawSadFace();
    strip.clear();
    playSongInFolder01(7);
    strip.setPixelColor(0, strip.Color(0, 255,0 ));
    strip.setPixelColor(1, strip.Color(0, 255,0 ));
    strip.setPixelColor(2, strip.Color(0, 255,0 ));
    strip.setPixelColor(3, strip.Color(0, 255,0 ));
    strip.setPixelColor(4, strip.Color(0, 255,0 ));
    strip.show();
    lastSignalTime = millis() + 300;
    currentState = PLAY;
  }

}

void won(){
  currentState = WON;
   int currentTime = millis();
  if(currentTime > lastSignalTime + 900)
  {
    send_game_time();
    drawSmileyFace();
    strip.clear();
    playSongInFolder01(6);
    strip.setPixelColor(0, strip.Color(255,0,0 ));
    strip.setPixelColor(1, strip.Color(255,0,0 ));
    strip.setPixelColor(2, strip.Color(255,0,0 ));
    strip.setPixelColor(3, strip.Color(255,0,0 ));
    strip.setPixelColor(4, strip.Color(255,0,0 ));
    strip.show();
    lastSignalTime = millis()+ 300;
    currentState = PLAY;
  }
}

void wonFin(){
  currentState = WON;
   int currentTime = millis();

  if(currentTime > lastSignalTime + 900)
  {
    drawSmileyFace();
    strip.clear();
    playSongInFolder01(6);
    strip.setPixelColor(0, strip.Color(255,0,0 ));
    strip.setPixelColor(1, strip.Color(255,0,0 ));
    strip.setPixelColor(2, strip.Color(255,0,0 ));
    strip.setPixelColor(3, strip.Color(255,0,0 ));
    strip.setPixelColor(4, strip.Color(255,0,0 ));
    strip.show();
    lastSignalTime = millis()+ 300;
    readBuffer = "";
    currentState = MENU;
    send_game_time();
  }
}

void lightLEDPressed(int ledNum)
{
  strip.clear();
  strip.setPixelColor(ledNum, strip.Color(5, 200,160 ));
  strip.show();
}


void readSequence()
{
  int currentTime = millis();
  if(currentTime > lastSignalTime + 500)
  {
    for(int button = 0; button<4 ; button++)
    {
    bool res = handle_button(button);
    if(res)
    {
      lightLEDPressed(button);
      if(sequence[readIndex] != button )
      {
        currentSequenceLength=1;
        readIndex=0;
        lastSignalTime = millis();
        playSongInFolder01(button+1);
        gameOver();
        
      }
      else
      {
        if(readIndex < currentSequenceLength - 1){
          playSongInFolder01(button+1);
          readIndex ++;
        }
        else{
          playSongInFolder01(button+1);
          currentSequenceLength ++;
          readIndex=0;
          lastSignalTime = millis();
          if(currentSequenceLength > 4)
          {
            currentSequenceLength = 1;
            wonFin();
            
          }
          else
          {
            won();
          }
          
          
        }
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
void stopSong() {
  byte command[] = {0x7E, 0x02, 0x16, 0xEF};
  mp3Serial.write(command, sizeof(command));  // Send the stop command to the MP3 board
}

void playSongInFolder01(int songNumber) {
  //stopSong();  // First, stop any currently playing song
  byte command[] = {0x7E, 0x04, 0x42, 0x01, static_cast<byte>(songNumber), 0xEF};
  mp3Serial.write(command, sizeof(command));  // Then, send the command to play the new song
}

void send_game_time() {
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - startTime; // startTime should be set when the game starts
  int secondsElapsed = timeElapsed / 1000;  // Convert milliseconds to seconds
  startTime = currentTime;
  String message = "end_game_time " + String(secondsElapsed);
  SerialBT.println(message);
}


// 1. Draws a smiley face
void drawSmileyFace() {
  matrix.clear();
  // Eyes
  matrix.setPoint(2, 2, true);
  matrix.setPoint(2, 5, true);

  // Mouth - smile
  matrix.setPoint(5, 1, true);
  matrix.setPoint(5, 6, true);
  matrix.setPoint(6, 2, true);
  matrix.setPoint(6, 3, true);
  matrix.setPoint(6, 4, true);
  matrix.setPoint(6, 5, true);

  matrix.update();
}

// 2. Draws a sad face
void drawSadFace() {
  matrix.clear();
  // Eyes
  matrix.setPoint(2, 2, true);
  matrix.setPoint(2, 5, true);

  // Mouth - sad
  matrix.setPoint(5, 1, true);
  matrix.setPoint(5, 6, true);
  matrix.setPoint(4, 2, true);
  matrix.setPoint(4, 3, true);
  matrix.setPoint(4, 4, true);
  matrix.setPoint(4, 5, true);

  matrix.update();
}

// 3. Draws a square
void drawSquare() {
  matrix.clear();
  // Horizontal lines
  for (int i = 0; i < 8; i++) {
    matrix.setPoint(0, i, true);
    matrix.setPoint(7, i, true);
  }

  // Vertical lines
  for (int j = 0; j < 8; j++) {
    matrix.setPoint(j, 0, true);
    matrix.setPoint(j, 7, true);
  }

  matrix.update();
}


