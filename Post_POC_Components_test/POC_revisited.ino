#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include "BluetoothSerial.h"
#include <HardwareSerial.h>

/******************************************************************
------------------------Constants Section-------------------------
*******************************************************************/
#define MAX_PRESS_TIME 2000
#define STRIP_PIN 2
#define NUM_LEDS 5
#define NUM_PINS 5
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 2
#define MAX_GAME_SEQUENCE 100
#define SLOW_PACE 900
#define FAST_PACE 600
/******************************************************************
------------------------Pin Layout Section-------------------------
*******************************************************************/
/**As the esp32, 30pin version's layout is not numbered according to position the pin numbering is confusing, even though our products connection is quite organized.
This is the order of the pinout(all the pins are connected to the right side of the board, except the GND and VIN as 5V was needed)

(From top to bottom[usb port])
*/
#define MATRIX_DATA_PIN 23
//button pins order is 22,1,3,21,19
//array's order indicates the logical order on the assembled case
const int buttonPins[NUM_PINS] = {22, 3, 21, 1, 19};

//notice the led matrix pins is seperated as changing the 23 pin caused some issues(this is the SPI standard pin for DATA_IN)
#define MATRIX_CLK_PIN 18
#define MATRIX_CS_PIN 5
byte numberPatterns[10][8] = {
  { 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00 },//0
  { 0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00 },  // 1
  { 0x3E, 0x02, 0x02, 0x3E, 0x20, 0x20, 0x3E, 0x00 },  // 2
  { 0x3E, 0x02, 0x02, 0x3E, 0x02, 0x02, 0x3E, 0x00 },  // 3
  { 0x22, 0x22, 0x22, 0x3E, 0x02, 0x02, 0x02, 0x00 },  // 4
  { 0x3E, 0x20, 0x20, 0x3E, 0x02, 0x02, 0x3E, 0x00 },  // 5
  { 0x3E, 0x20, 0x20, 0x3E, 0x22, 0x22, 0x3E, 0x00 },  // 6
  { 0x3E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00 },  // 7
  { 0x3E, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x3E, 0x00 },  // 8
  { 0x3E, 0x22, 0x22, 0x3E, 0x02, 0x02, 0x3E, 0x00 }   // 9
};
//UART pinout for mp3 board
#define MP3_SERIAL_RX 16  // Connect to MP3 board TX
#define MP3_SERIAL_TX 17  // Connect to MP3 board RX

#define MIN_PRESS_TIME 40
MD_Parola parola = MD_Parola(HARDWARE_TYPE, MATRIX_CS_PIN, MAX_DEVICES);
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, MATRIX_DATA_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, MAX_DEVICES);

//pin number 4 is skipped for easier assembly

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);




#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

String readBuffer = ""; // Buffer to store incoming characters to check for the "light_led" string.

HardwareSerial mp3Serial(2); // Use HardwareSerial 2 for ESP32

enum GAME_STATE{
  PLAY,
  READ,
  WON,
  LOST,
  MENU
};

GAME_STATE currentState  =MENU;

//global variables for buttons
bool currentButtonStates [] = {true, true,true,true,true}; 
bool previousButtonStates [] = {true, true,true,true,true}; //
bool previousButtonState = HIGH; // will store last time button was updated
unsigned int buttonPress_counters [] = { 0 , 0 ,0,0,0}; //number of presses
unsigned long buttonPress_last_times [] = { 0 , 0 ,0,0,0};   // last time button was pressed
unsigned long last_press_duration= 0; //to allow accessing last press duration as global
int lastSignalTime = 0;
int playIndex = 0;
int readIndex = 0;
unsigned long startTime;
byte sequence[100] ;
bool isLighting = true;
int currentSequenceLength = 1;

//Customable settings section
int sound_set = 1; //by default the game uses the first set
bool is_fast = false; // defines easy mode (600 or 900)
bool is_min_buttons;//(randomizer picks only 3 buttons if toggled)
int color_set = 1;
int first_led_colors[3] = {0,0,255}; //default is blue
int second_led_colors[3] = {255,87,51}; //default is yellow
int third_led_colors[3] = {255,0,0}; //default is red
int fourth_led_colors[3] = {0,255,0}; //default is green



void setup() {
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

void offline_start()
{
  unsigned long currentTime = millis();  // Correct the type for millis()
  if(currentTime > lastSignalTime + 900)
  {
      currentState = PLAY;
      startTime = millis();
  }
}
void checkCommand() {
 unsigned long currentTime = millis();
    
    if(currentTime > lastSignalTime + 900) {
        lastSignalTime = currentTime;
        
        // If the buffer is too long, trim it.
        if (readBuffer.length() > 100) {
            readBuffer = readBuffer.substring(readBuffer.length() - 100);
        }
        
        // Check if buffer contains the command "start_game_"
        int commandIndex = readBuffer.indexOf("start_game_");
        
        if (commandIndex != -1) {
            // Extract the command from the buffer
            String command = readBuffer.substring(commandIndex);
            
            // Tokenize the command using underscore as a delimiter
            int numTokens = 5;  // "start_game", A, B, C, D
            String tokens[numTokens];
            
            for (int i = 0; i < numTokens; i++) {
                int underscoreIndex = command.indexOf('_');
                
                if (underscoreIndex != -1) {
                    tokens[i] = command.substring(0, underscoreIndex);
                    command = command.substring(underscoreIndex + 1);
                } else {
                    tokens[i] = command;
                }
            }

            // Evaluate the tokens
            if (tokens[1] == "fast_mode") {
                is_fast = true;
            } else if (tokens[1] == "slow_mode") {
                is_fast = false;
            }

            if (tokens[2] == "3_lights") {
                is_min_buttons = true;
            } else if (tokens[2] == "4_lights") {
                is_min_buttons = false;
            }

            if (tokens[3] == "sound_set_1") {
                sound_set = 1;
            } else if (tokens[3] == "sound_set_2") {
                sound_set = 2;
            } else if (tokens[3] == "sound_set_const") {
                sound_set = 3;
            }

            if (tokens[4] == "color_set_1") {
                color_set = 1;
            } else if (tokens[4] == "color_set_2") {
                color_set = 2;
            } else if (tokens[4] == "color_set_3") {
                color_set = 3;
            }

            // Clear the buffer to avoid re-processing
            readBuffer = "";
            clearBTBuffer();

            currentState = PLAY;
            startTime = millis();
        }
    }
}

void clearBTBuffer() {
  while (SerialBT.available() > 0) {
    char t = SerialBT.read();
  }
}

void loop() 
{
  //sound_board_bench();
  switch(currentState)
  {
    case MENU:
        while (Serial.available()) {
          char c = Serial.read();
          SerialBT.write(c);  
          readBuffer += c;
          checkCommand();
        }

        // Read and forward data from Bluetooth Serial to wired Serial
        while (SerialBT.available()) {
          char c = SerialBT.read();
          Serial.write(c);  
          readBuffer += c;
          checkCommand();
        }

        if(handle_button(0))
          offline_start();
  
      
      break;
    case PLAY:
      playSequence();
      break;
    case READ:
      readSequence();
      break;
    case WON:
    setVolumeMax();
      won();
      break;
    case LOST:
      gameOver();
      break;
    default:
    break;
  }
    
}


void playSequence()
{
  if(millis() > lastSignalTime +700)
  {
    
      displayTwoDigitNumber(currentSequenceLength-1);
    if(isLighting){
      sequence[currentSequenceLength-1] = next_led();
      lightLED(sequence[playIndex]);
      playSongInFolder(sound_set,sequence[playIndex]);
    playIndex +=1;
    lastSignalTime = millis();
    isLighting = false;
    }
    else
    {
      lastSignalTime = millis();
      isLighting = true;
      clearLed();
      if(playIndex >= currentSequenceLength)
    {
      currentState = READ;
      playIndex = 0;
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
    playSongInFolder(sound_set,7);
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
    playSongInFolder(sound_set,6);
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
    playSongInFolder(sound_set,6);
    strip.setPixelColor(0, strip.Color(255,0,0 ));
    strip.setPixelColor(1, strip.Color(255,0,0 ));
    strip.setPixelColor(2, strip.Color(255,0,0 ));
    strip.setPixelColor(3, strip.Color(255,0,0 ));
    strip.setPixelColor(4, strip.Color(255,0,0 ));
    strip.show();
    lastSignalTime = millis()+ 300;
    readBuffer = "";
    clearBTBuffer();
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
    for(int button = 0; button<5 ; button++)
    {
    bool res = handle_button(button);
    if(res)
    {
      lightLED(button);
      //lightLEDPressed(button);
      if(sequence[readIndex] != button )
      {
        currentSequenceLength=1;
        readIndex=0;
        lastSignalTime = millis();
        playSongInFolder(sound_set,button);
        gameOver();
        
      }
      else
      {
        if(readIndex < currentSequenceLength - 1){
          playSongInFolder(sound_set,button);
          readIndex ++;
        }
        else{
          playSongInFolder(sound_set,button);
          currentSequenceLength ++;
          readIndex=0;
          lastSignalTime = millis();
          if(currentSequenceLength > MAX_GAME_SEQUENCE)
          {
            currentSequenceLength = 1;
            wonFin();
            
          }
          else
          {
            //won();
            send_game_stage();
            lastSignalTime = millis()+ 300;
    currentState = PLAY;

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
  switch(ledNum)
  {
    case 1:
    strip.setPixelColor(ledNum, strip.Color(first_led_colors[0], first_led_colors[1],first_led_colors[2] ));
    break;
    case 2:
    strip.setPixelColor(ledNum, strip.Color(second_led_colors[0], second_led_colors[1],second_led_colors[2] ));
    break;
    case 3:
    strip.setPixelColor(ledNum, strip.Color(third_led_colors[0], third_led_colors[1],third_led_colors[2] ));
    break;
    case 4:
    strip.setPixelColor(ledNum, strip.Color(fourth_led_colors[0], fourth_led_colors[1],fourth_led_colors[2] ));
    break;
    //this is the control led
    strip.setPixelColor(ledNum, strip.Color(0,255,0));
    default:

    break;
  }
  
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

void playSongInFolder(int folderNumber, int songNumber) {
    if (folderNumber < 1 || folderNumber > 99 || songNumber < 0 || songNumber > 255) {
        // Add error handling for invalid folder or song numbers, if desired
        return;
    }
    
    //stopSong();  // First, stop any currently playing song

    byte command[] = {0x7E, 0x04, 0x42, static_cast<byte>(folderNumber), static_cast<byte>(songNumber), 0xEF};
    mp3Serial.write(command, sizeof(command));  // Send the command to play the new song
}


void send_game_time() {
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - startTime; // startTime should be set when the game starts
  int secondsElapsed = timeElapsed / 1000;  // Convert milliseconds to seconds
  startTime = currentTime;
  String message = "end_game_time " + String(secondsElapsed);
  SerialBT.println(message);
}

void send_game_stage() {
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - startTime; // startTime should be set when the game starts
  int secondsElapsed = timeElapsed / 1000;  // Convert milliseconds to seconds
  startTime = currentTime;
  String message = "game_stage " + String(currentSequenceLength);
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

void setVolumeMax() {
  byte volumeMaxCmd[] = {0x7E, 0x03, 0x31, 0x1E, 0xEF}; // Command to set volume to max (30)
  mp3Serial.write(volumeMaxCmd, sizeof(volumeMaxCmd));  // Send the command to the mp3 module
}

void sound_board_bench()
{
  while(true)
  {
    for(int i = 0 ;i<4 ; i++)
    {
     // playSongInFolder01(i);
      delay(300);
      stopSong();
      delay(300);
    }
    
  }
  
}

void displayDigit(int num, int zone) {
  if (num < 0 || num > 9) {
    matrix.clear(zone);
    return;
  }

  for (int row = 0; row < 8; row++) {
    matrix.setRow(zone, row, numberPatterns[num][row]);
  }
}

void displayTwoDigitNumber(int num) {
  

  if (num < 10) {  // If it's a single-digit number
    matrix.clear(0);  // Clear the left matrix
    displayDigit(num, 1);  // Display the number on the right matrix
  } else {
    int leftDigit = num / 10;   // Extract tens place
    int rightDigit = num % 10; // Extract ones place

    displayDigit(leftDigit, 0); // Display left digit on left matrix
    displayDigit(rightDigit, 1); // Display right digit on right matrix
  }
}

int next_led()
{
  if(is_min_buttons)
    return random(1,4);
  else
    return random(1,5);
}

