// Define LED pins
#define RED_LED 27
#define GREEN_LED 25
#define BLUE_LED 26

// Define button pins
#define BUTTON_GREEN 32
#define BUTTON_BLUE 33

void setup() {
  // Initialize LED pins as OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Turn off the LED initially
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);

  // Initialize button pins with internal pull-up resistors
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_GREEN) == LOW) {  // Button for Green pressed
    turnAllLedsOff();
    digitalWrite(GREEN_LED, LOW);  // Turn on the GREEN LED
    delay(100);  // Simple debouncing
  } 

  if (digitalRead(BUTTON_BLUE) == LOW) {  // Button for Blue pressed
    turnAllLedsOff();
    digitalWrite(BLUE_LED, LOW);  // Turn on the BLUE LED
    delay(100);  // Simple debouncing
  }
}

void turnAllLedsOff() {
  // Turn off all LEDs
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}