#include <HardwareSerial.h>

#define MP3_SERIAL_RX 16  // Connect to MP3 board TX
#define MP3_SERIAL_TX 17  // Connect to MP3 board RX

HardwareSerial mp3Serial(2); // Use HardwareSerial 2 for ESP32
int currentSong = 1;

void setup() {
  Serial.begin(115200);  // Begin the primary serial for debugging
  mp3Serial.begin(9600, SERIAL_8N1, MP3_SERIAL_RX, MP3_SERIAL_TX); // Begin the secondary serial to communicate with MP3 board
  
  delay(1000);  // Give a second for everything to power up properly
  
  playSongInFolder01(currentSong);  // Play the first song in folder "01"
}

void loop() {
  delay(1000); // Play for 1 second
  currentSong++;

  if (currentSong > 5) {
    currentSong = 1;
    delay(5000); // wait for 5 seconds before starting over
  }

  playSongInFolder01(currentSong); // play next song
}

void playSongInFolder01(int songNumber) {
  byte command[] = {0x7E, 0x04, 0x42, 0x01, static_cast<byte>(songNumber), 0xEF};
  mp3Serial.write(command, sizeof(command));  // Send the command to the MP3 board
}
