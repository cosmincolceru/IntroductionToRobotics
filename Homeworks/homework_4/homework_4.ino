// Program that simulates a stopwatch using 3 buttons and a 4 digit 7 segment display
// The buttons are used for starting/stoping the stopwatch, reseting, saving a lap and viewing saved laps

#define SHORT_PRESS 1
#define CONTINUOUS_PRESS 2

// Define connections to the shift register
const int latchPin = 11;  // Connects to STCP (latch pin) on the shift register
const int clockPin = 10;  // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;   // Connects to DS (data pin) on the shift register

// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

// Store the digits in an array for easy access
int displayDigits[] = { segD1, segD2, segD3, segD4 };

const int displayCount = 4;  // Number of digits in the display

// Define the number of unique encodings (0-9)
const int encodingsNumber = 10;

unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long seconds = 0;
unsigned long milliseconds = 0;

bool counterStarted = false;
bool lapMode = false;

byte byteEncodings[encodingsNumber] = {
//A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
};

class Button {
private:
  int pin;
  byte state;
  byte reading;
  byte lastReading;
  unsigned long pressTime;
  unsigned long releaseTime;
  unsigned long lastDebounceTime;
  static unsigned long debounceDelay;
public:
  Button(int pin) {
    this->pin = pin;
    this->state = LOW;
    this->reading = LOW;
    this->lastReading = LOW;

    pinMode(this->pin, INPUT_PULLUP);
  }

  Button() {
    this->state = LOW;
    this->reading = LOW;
    this->lastReading = LOW;
  }

  void setPin(int pin) {
    this->pin = pin;
    pinMode(this->pin, INPUT_PULLUP);
  }

  int complexPress() {
    this->reading = !digitalRead(this->pin);

    long duration;

    if (this->reading != this->lastReading) {
      this->lastDebounceTime = millis();
    }

    if (millis() - this->lastDebounceTime > debounceDelay) {
      if (this->reading != this->state) {
        this->state = this->reading;

        if (this->state == HIGH) {
          this->pressTime = millis();
          Serial.println("pressed");
        }
        if (this->state == LOW) {
          this->releaseTime = millis();
          Serial.println("released");
          duration = this->releaseTime - this->pressTime;
          Serial.println(duration);
        
        }
      }
    }
    this->lastReading = this->reading;
    if (duration < 1000) {
      Serial.println("Short press");
      return SHORT_PRESS;
    }

    if (this->state == HIGH) {
      if (millis() - this->pressTime >= 1000) {
        return CONTINUOUS_PRESS;
      }
    }
    return 0;
  }

  bool pressed() {
    this->reading = !digitalRead(this->pin);

    if (this->reading != this->lastReading) {
      this->lastDebounceTime = millis();
    }

    if (millis() - this->lastDebounceTime > debounceDelay) {
      if (this->reading != this->state) {
        this->state = this->reading;

        if (this->state == HIGH) {
          this->lastReading = this->reading;
          return true;
        }
      }
    }
    this->lastReading = this->reading;
    return false;
  }
};

unsigned long Button::debounceDelay = 50;

Button startButton(3);
Button resetButton(2);
Button lapButton(8);

struct Time {
  int seconds;
  int milliseconds;
};

const int maxLaps = 4;  // maximum number of laps that can be saved
Time laps[maxLaps];

int lapIndex = 0; // index where the next lap will be written
int nrLaps = 0;   // total number of laps saved
int lapDisplayIndex = 0;  // index of the lap that is currently displayed

// variables for cycling through the laps continuously
unsigned long lastChange = 0;
unsigned long changeTime = 1000;

void setup() {
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  
  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}

void loop() {
  // If not in lap mode, pressing the start button will start or pause the counter
  if (!lapMode && startButton.pressed()) {
    Serial.println("Start/Stop");
    counterStarted = !counterStarted;
  }

  // If the counter is paused and the reset button is pressed, reset the counter
  if (!lapMode && !counterStarted && resetButton.pressed()) {
    Serial.println("Reset counter");
    milliseconds = 0;
    seconds = 0;
  }

  // If the counter if started the lap button is pressed save the current time
  if (counterStarted && lapButton.pressed()) {
    Serial.println("Save lap");
    // Save the current lap
    laps[lapIndex].seconds = seconds;
    laps[lapIndex].milliseconds = milliseconds;

    // Increment the lap index, if the index overflows go back to 0
    lapIndex = (lapIndex + 1) % maxLaps;

    // Save the total number of laps 
    nrLaps = min(nrLaps + 1, maxLaps);
  }

  // If in lap mode and the lap button is pressed, go to the next lap
  if (lapMode && lapButton.complexPress() == SHORT_PRESS) {
    Serial.println("Go to next lap");
    lapDisplayIndex = (lapDisplayIndex + 1) % nrLaps;
  }

  if (lapMode && lapButton.complexPress() == CONTINUOUS_PRESS) {
    if (millis() - lastChange > changeTime) {
      lapDisplayIndex = (lapDisplayIndex + 1) % nrLaps;
      lastChange = millis();
    }
  }

  // If the counter is reseted, lap button is pressed and there are saved laps, set the stopwatch to lap mode
  if (!lapMode && !counterStarted && seconds == 0 && milliseconds == 0 && lapButton.complexPress() == SHORT_PRESS && nrLaps > 0) {
    Serial.println("Set lap mode");
    lapMode = true;
  }

  if (!lapMode && lapButton.complexPress() == CONTINUOUS_PRESS && !counterStarted && seconds == 0 && milliseconds == 0 && nrLaps > 0) {
    lapMode = true;
    if (millis() - lastChange > changeTime) {
      lapDisplayIndex = (lapDisplayIndex + 1) % nrLaps;
      lastChange = millis();
    }
  }

  // If the stopwatch is in lap mode and the reset button is pressed, "delete" the saved laps and unset lap mode
  if (lapMode && resetButton.pressed()) {
    Serial.println("Reset laps");
    lapMode = false;
    nrLaps = 0;
    lapIndex = 0;
    lapDisplayIndex = 0;
  }

  if (counterStarted) {
    if (millis() - lastIncrement > delayCount) {
      milliseconds++;
      if (milliseconds == 10) {
        milliseconds = 0;
        seconds++;
      }
      seconds %= 1000;  // Wrap around after 999
      lastIncrement = millis();
    }
  }

  if (lapMode) {
    writeNumber(laps[lapDisplayIndex].seconds, laps[lapDisplayIndex].milliseconds);
  } 
  else {
    writeNumber(seconds, milliseconds);
  }
}

void writeReg(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number, int milliseconds) {
  int currentNumber = number;
  int currentMillis = milliseconds;

  int displayDigit = 3;  // Start with the milliseconds display
  activateDisplay(displayDigit);
  writeReg(byteEncodings[currentMillis]);

  displayDigit--;       // Got to the next display
  writeReg(B00000000);  // Clear the register to avoid ghosting

  int lastDigit = 0;
  while (displayDigit >= 0) {
    lastDigit = currentNumber % 10;
    byte currentEncoding = byteEncodings[lastDigit];

    // Activate the decimal point for the last digit in the number
    if (displayDigit == 2) {
      currentEncoding += 1;
    }

    activateDisplay(displayDigit);
    writeReg(currentEncoding);

    displayDigit--;
    currentNumber /= 10;
    writeReg(B00000000);  // Clear the register to avoid ghosting
  }
}
