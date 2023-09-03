#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const int ledPin = 2;  // This is usually the internal LED pin for ESP32. Adjust if necessary.
bool ledState = false; // To track the current state of the LED.
String readBuffer = ""; // Buffer to store incoming characters to check for the "light_led" string.

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  pinMode(ledPin, OUTPUT); // Set the LED pin as output
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  // Read and forward data from wired Serial to Bluetooth Serial
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
}

void checkCommand() {
  if (readBuffer.length() > 100) {  // Trim the buffer if it gets too long.
    readBuffer = readBuffer.substring(readBuffer.length() - 100);
  }

  if (readBuffer.indexOf("light_led") != -1) {  // If "light_led" is found in the buffer...
    ledState = !ledState;  // Toggle LED state
    digitalWrite(ledPin, ledState ? HIGH : LOW);  // Turn LED on/off based on the new state
    readBuffer = "";  // Clear the buffer to avoid processing the same command multiple times.
  }
}
