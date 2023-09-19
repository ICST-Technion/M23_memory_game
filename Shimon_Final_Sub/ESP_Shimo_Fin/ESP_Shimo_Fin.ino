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
#define SLOW_PACE 1100
#define FAST_PACE 700
#define PRESS_BLINK_DURATION 400
#define RESET_INTERVAL 400
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
unsigned long last_read_press = 0;
bool fin_press_cleared = false;
bool toggle_reset = false;
bool toggle_text = false;
bool wating_correct_button_approval = false;
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


int next_led()
{
  if(is_min_buttons)
    return random(1,4);
  else
    return random(1,5);
}

void set_color_set_1()
{
    first_led_colors[0] = 0;      // Blue
    first_led_colors[1] = 0;
    first_led_colors[2] = 255;
    
    second_led_colors[0] = 255;  // Yellow
    second_led_colors[1] = 87;
    second_led_colors[2] = 51;
    
    third_led_colors[0] = 255;   // Red
    third_led_colors[1] = 0;
    third_led_colors[2] = 0;
    
    fourth_led_colors[0] = 0;    // Green
    fourth_led_colors[1] = 255;
    fourth_led_colors[2] = 0;
}

void set_color_set_2()
{
    first_led_colors[0] = 239;   // Purple
    first_led_colors[1] = 0;
    first_led_colors[2] = 255;
    
    second_led_colors[0] = 255;  // Orange
    second_led_colors[1] = 111;
    second_led_colors[2] = 0;
    
    third_led_colors[0] = 255;   // Pink
    third_led_colors[1] = 0;
    third_led_colors[2] = 179;
    
    fourth_led_colors[0] = 139;  // Brown
    fourth_led_colors[1] = 69;
    fourth_led_colors[2] = 19;
}

void set_color_set_3()
{
    first_led_colors[0] = 0;     // Cyan
    first_led_colors[1] = 255;
    first_led_colors[2] = 247;
    
    second_led_colors[0] = 255;  // Red
    second_led_colors[1] = 0;
    second_led_colors[2] = 68;
    
    third_led_colors[0] = 239;   // Purple
    third_led_colors[1] = 0;
    third_led_colors[2] = 255;
    
    fourth_led_colors[0] = 255;  // Yellow
    fourth_led_colors[1] = 87;
    fourth_led_colors[2] = 51;
}


void set_color_set(int set)
{
  switch(set)
  {
    case 1:
    set_color_set_1();
    break;
    case 2:
    set_color_set_2();
    break;
    case 3:
    set_color_set_3();
    break;
    default:
    break;

  }
}

void setup() {
  Serial.begin(115200);  // Begin the primary serial for debugging
  mp3Serial.begin(9600, SERIAL_8N1, MP3_SERIAL_RX, MP3_SERIAL_TX); // Begin the secondary serial to communicate with MP3 boa
  strip.begin();
  strip.show();
  SerialBT.begin("ESP32test"); //Bluetooth device name
  parola.begin();
  parola.displayClear();
  parola.setIntensity(15);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  set_color_set(1);
}

void offline_start()
{
  unsigned long currentTime = millis();  // Correct the type for millis()
  if(currentTime > lastSignalTime + FAST_PACE)
  {
      currentState = PLAY;
      startTime = millis();
  }
}

void startGame() {
      readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
      clearBTBuffer();  // Flush the hardware buffer to make sure no extra characters are left
      currentState = PLAY;
      startTime = millis();
}
void checkCommand() {
  unsigned long currentTime = millis();  // Correct the type for millis()
  if(currentTime > lastSignalTime + 900)
  {
    // ... Your existing code for populating readBuffer ...

    if (readBuffer.length() > 100) {  // Trim the buffer if it gets too long.
      readBuffer = readBuffer.substring(readBuffer.length() - 100);
    }

    if (readBuffer.indexOf("start_game") != -1) {  // If "start_game" is found in the buffer...
      readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
      clearBTBuffer();  // Flush the hardware buffer to make sure no extra characters are left
      currentState = PLAY;
      startTime = millis();
    }

    else
    {
      String message =readBuffer;
      //readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
      //clearBTBuffer();  // Read the received message
      if (message.indexOf("slow_mode") != -1){
          is_fast = false;
          
      }
      if (message.indexOf("fast_mode") != -1){
          is_fast = true;
          //drawSadFace();
      }
      if (message.indexOf("3_lights") != -1){
          is_min_buttons = true;
         
      }
      if (message.indexOf("4_lights") != -1){
          is_min_buttons = false;
          // readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
          // clearBTBuffer();
          //drawSadFace();
      }
      if (message.indexOf("color_set_1") != -1){
          color_set = 1;
          readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
          clearBTBuffer();
      }
      if (message.indexOf("color_set_2") != -1){
          color_set = 2;
          readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
          clearBTBuffer();
      }
      if (message.indexOf("color_set_3") != -1){
          color_set = 3;
          readBuffer = "";  // Clear the string buffer to avoid processing the same command multiple times.
          clearBTBuffer();
      }
      //displayTwoDigitNumber(color_set);
      set_color_set(color_set);
      // if(color_set == 3){
      //   drawSadFace();
      // }
      // if(color_set == 2){
      
      // }
   
    }
  }
}



void clearBTBuffer() {
  while (SerialBT.available() > 0) {
    char t = SerialBT.read();
  }
}

void check_full_reset()
{
  for(int i = 0; i < 5 ; i++)
  {
    if(digitalRead(buttonPins[i]) == HIGH)
      return;
  }
  ESP.restart();
}




void playSequence()
{
  toggle_text = false;
  int interval=FAST_PACE;
  if (is_fast==false){
    interval=SLOW_PACE;
  }

  if(last_read_press + PRESS_BLINK_DURATION < millis() && !fin_press_cleared)
  {
    fin_press_cleared = true;
    clearLed();
  }
    
  if(millis() > lastSignalTime + interval)
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
    //drawSadFace();
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
    if(last_read_press + PRESS_BLINK_DURATION < currentTime)
    {
      clearLed();
    }
    
        for(int button = 1; button<5 ; button++)
      {
        if(button == sequence[readIndex])
          continue;
        read_button_method(button);
      }
      //giving the intended correct button the least priority
      read_button_method(sequence[readIndex]);
    
    
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

bool handle_button(uint8_t button_index) {
    bool previousButtonState = previousButtonStates[button_index];
    bool currentButtonState = digitalRead(buttonPins[button_index]);

    if (currentButtonState != previousButtonState)  //if change is detected
    {
        previousButtonStates[button_index] = currentButtonState;

        if (currentButtonState == LOW && previousButtonState == HIGH)  //type of change - from released to pressed
        {
            buttonPress_last_times[button_index] = millis();
        }
        else {
            unsigned long last_press_duration = millis() - buttonPress_last_times[button_index];

            // Check for a valid button press duration between MIN and MAX times
            if (last_press_duration > MIN_PRESS_TIME && last_press_duration < MAX_PRESS_TIME) {
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
  String message = "game_stage " + String(currentSequenceLength -1);
  SerialBT.println(message);
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


void displayTwoDigitNumber(int num) {
  

 // Ensure the number is between 0 and 99
  if (num < 0) num = 0;
  if (num > 99) num = 99;
  
  char buffer[3];  // Buffer to hold the converted number
  sprintf(buffer, "%02d", num);  // Convert the integer to a string with zero padding
  
  parola.displayText(buffer, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  parola.displayAnimate();
  
}





void checkReset()
{
  if(handle_button(0))
  {
    lastSignalTime = millis();
    resetGame();
  }
}
void resetGame()
{
  currentState = MENU;
  playIndex = 0;
  readIndex = 0;
  currentSequenceLength = 1;
  int currentTime = millis();
  

  if(currentTime > lastSignalTime + 900)
  {

    strip.clear();
    playSongInFolder(sound_set,6);
    strip.setPixelColor(0, strip.Color(0,0,255));
    strip.setPixelColor(1, strip.Color(0,0,255));
    strip.setPixelColor(2, strip.Color(0,0,255));
    strip.setPixelColor(3, strip.Color(0,0,255));
    strip.setPixelColor(4, strip.Color(0,0,255));
    strip.show();
    lastSignalTime = millis()+ 300;
    readBuffer = "";
    clearBTBuffer();
    currentState = MENU;
    send_game_time();
  }
}



void read_button_method(int button)
{
   bool res = handle_button(button);
    if(res)
    {
      last_read_press = millis();
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
            fin_press_cleared = false;
    currentState = PLAY;

          }
          
          
        }
      }
    }

}

void loop() 
{
  check_full_reset();
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
         // String message = SerialBT.readString(); // Read the received message
        // Serial.println(message); // Print the received message to the Serial Monitor
          char c = SerialBT.read();
          Serial.write(c);  
          readBuffer += c;
          checkCommand();
        }

        if(handle_button(0) && lastSignalTime + RESET_INTERVAL < millis())
          offline_start();
        if(!toggle_text)
        {
          parola.displayText("STBY", PA_LEFT, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          toggle_text = true;
        }
          
         if (parola.displayAnimate()) {
          parola.displayReset();
  }

      
      break;
    case PLAY:
      playSequence();
      checkReset();
      break;
    case READ:
      readSequence();
      checkReset();
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