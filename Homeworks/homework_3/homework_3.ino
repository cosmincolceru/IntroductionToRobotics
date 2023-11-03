#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

unsigned long blinkInterval = 500;
unsigned long previousTime = 0;

// joystick pins
const int pinSW = 2; 
const int pinX = A0;
const int pinY = A1;

// declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int segSize = 8;

bool commonAnode = false; // Modify if you have common anode

byte lastReading = LOW;
byte swState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
//  0     1     2     3     4     5     6     7
};

byte segmentStates[segSize] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

const int noOfDirections = 4;

int moveMatrix[segSize][noOfDirections] = {
// up   down  left  right 
  {-1,   6,    5,    1}, // a
  { 0,   6,    5,   -1}, // b
  { 6,   3,    4,    7}, // c
  { 6,  -1,    4,    2}, // d
  { 6,   3,   -1,    2}, // e
  { 0,   6,   -1,    1}, // f
  { 0,   3,   -1,   -1}, // g
  {-1,  -1,    2,   -1}, // dp
};

int currentSegment = 7;
byte currentSegmentState = HIGH;

int xValue = 0;
int yValue = 0;

bool joyMoved = false;

int minThreshold = 200;
int maxThreshold = 800;

const int longPressTime = 1000;

unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

void setup() {
  Serial.begin(9600);
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }

  pinMode(pinSW, INPUT_PULLUP);
  
  if (commonAnode == true) {
    for (int i = 0; i < segSize; i++) {
      segmentStates[i] = !segmentStates[i];
    }
  }
}

void loop() {
  unsigned long currentTime = millis();

  // read the values from the potentiometers
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  
  // check if the joystick moved up, down, left or right
  if (xValue > maxThreshold && joyMoved == false) {
    if (moveMatrix[currentSegment][UP] != -1) {
      digitalWrite(segments[currentSegment], LOW);
      currentSegment = moveMatrix[currentSegment][UP];
    }
    joyMoved = true;
  }
  if (xValue < minThreshold && joyMoved == false) {
    if (moveMatrix[currentSegment][DOWN] != -1) {
      digitalWrite(segments[currentSegment], LOW);
      currentSegment = moveMatrix[currentSegment][DOWN];
    }
    joyMoved = true;
  }  
  if (yValue < minThreshold && joyMoved == false) {
    if (moveMatrix[currentSegment][LEFT] != -1) {
      digitalWrite(segments[currentSegment], LOW);
      currentSegment = moveMatrix[currentSegment][LEFT];
    }
    joyMoved = true;
  }
  if (yValue > maxThreshold && joyMoved == false) {
    if (moveMatrix[currentSegment][RIGHT] != -1) {
      digitalWrite(segments[currentSegment], LOW);
      currentSegment = moveMatrix[currentSegment][RIGHT];
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }

  // read the button state
  byte reading = !digitalRead(pinSW);

  // debounce the button and check if the press is long or short
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != swState) {
      swState = reading;
      if (swState == HIGH) {
        // button is pressed
        pressedTime = millis();
      }
      if (swState == LOW) {
        // button is released
        releasedTime = millis();

        // check how long the button press was
        long duration = releasedTime - pressedTime;
      
        if (duration < longPressTime) {
          // if it was a short press change the state of the current segment
          segmentStates[currentSegment] = !segmentStates[currentSegment];
        }
        else {
          // if it was a long press reset all segments and move to dp
          for (int i = 0; i < segSize; i++) {
            segmentStates[i] = LOW;
            currentSegment = 7;
          }
        }
      }
    }
  }
  lastReading = reading;

  for (int i = 0; i < segSize; i++) {
    if (i != currentSegment) {
      digitalWrite(segments[i], segmentStates[i]);
    }
  }

  // blink the current segment
  if (currentTime - previousTime > blinkInterval) {
    currentSegmentState = !currentSegmentState;
    previousTime = currentTime;
  }
  digitalWrite(segments[currentSegment], currentSegmentState);
}

