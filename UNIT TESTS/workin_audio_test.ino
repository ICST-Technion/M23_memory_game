#include <HardwareSerial.h>

#define MP3_SERIAL_RX 16  // Connect to MP3 board TX
#define MP3_SERIAL_TX 17  // Connect to MP3 board RX

HardwareSerial mp3Serial(2); // Use HardwareSerial 2 for ESP32

void setup() {
  Serial.begin(115200);  // Begin the primary serial for debugging
  mp3Serial.begin(9600, SERIAL_8N1, MP3_SERIAL_RX, MP3_SERIAL_TX); // Begin the secondary serial to communicate with MP3 board
  
  delay(1000);  // Give a second for everything to power up properly
  
  playFirstSongInFolder01();  // Play the first song in folder "01"
}

void loop() {
  // Empty for now
}

void playFirstSongInFolder01() {
  byte command[] = {0x7E, 0x04, 0x42, 0x01, 0x01, 0xEF};  // Command to play the song with the directory: /01/001xxx.mp3
  mp3Serial.write(command, sizeof(command));  // Send the command to the MP3 board
}
