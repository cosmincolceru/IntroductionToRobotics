
unsigned long previousBlinkTime = 0;
unsigned long previousChangeFloorTime = 0;
unsigned long previousOnFloorTime = 0;

const int blinkInterval = 1000;
const int changeFloorInterval = 3000;
const int onFloorTime = 1000;

unsigned long dingTime = 0;
unsigned long doorsClosingTime = 0;

bool onFloor = true;

// pins
const int elevatorLedPin = 10;
const int buttonPins[3] = {6, 7, 8};
const int floorLedPins[3] = {2, 3, 4};
const int buzzerPin = 11;

// led states
byte floorLedStates[3] = {HIGH, LOW, LOW};
byte elevatorLedState = HIGH;

bool elevatorMoving = false;
int currentFloor = 0;
int destination = 0;


const int elevatorMovingTone = 250;
const int dingTone = 2000;
const int doorsClosingTone = 1000;

const int dingDelay = 1000;
const int doorsClosingDelay = 700;

class Button {
private:
  int pin;
  byte state;
  byte reading;
  byte lastReading;
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

class Queue {
private:
  struct Node {
    int data;
    Node *next;
  };

  Node *front;
  Node *rear;
  int size;

public:
  Queue() {
    front = NULL;
    rear = NULL;
    size = 0;
  }

  void push(int value) {
    Node *newNode = new Node{value, NULL};

    if (rear == NULL) {
      front = rear = newNode;
    }
    else {
      rear->next = newNode;
      rear = newNode;
    }

    size++;
  }

  int pop() {
    Node *temp = front;
    int data = temp->data;
    front = front->next;
    delete temp;
    size--;

    if (front == NULL) {
      rear = NULL;
    }

    return data;
  }

  bool isEmpty() {
    return size == 0;
  }

  int getRear() {
    return rear->data;
  }

  ~Queue() {
    while (front != NULL) {
        Node* temp = front;
        front = front->next;
        delete temp;
    }
  }

};

unsigned long Button::debounceDelay = 50;

Button buttons[3];

Queue memory;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 3; i++) {
    buttons[i].setPin(buttonPins[i]);
    pinMode(floorLedPins[i], OUTPUT);
    digitalWrite(floorLedPins[i], floorLedStates[i]);
  }

  pinMode(elevatorLedPin, OUTPUT);
  digitalWrite(elevatorLedPin, elevatorLedState);
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentFloor == destination) {
    elevatorMoving = false;
  }
  
  // blink the elevator led if the elevator is moving, otherwise keep the led on
  if (elevatorMoving) {
    // blink the led
    if (currentTime - previousBlinkTime >= blinkInterval) {
      elevatorLedState = !elevatorLedState;
      previousBlinkTime = currentTime;
    }

    // wait for the doors to close
    if (currentTime - doorsClosingTime > doorsClosingDelay) { 
      // if the elevator is on a floor keep that floor's led on for 1 second
      if (onFloor) {
        if (currentTime - previousOnFloorTime >= onFloorTime) {
          floorLedStates[currentFloor] = LOW;
          previousChangeFloorTime = millis();
          onFloor = false;

          // play the elevator moving sound
          tone(buzzerPin, elevatorMovingTone);
        }
      }
      else {
        // if it is not on a floor, got to the next floor
        if (currentTime - previousChangeFloorTime >= changeFloorInterval) {
          if (currentFloor < destination) {
            currentFloor++;
          }
          else {
            currentFloor--;
          }
          previousChangeFloorTime = currentTime;
          floorLedStates[currentFloor] = HIGH;
          previousOnFloorTime = millis();
          onFloor = true;
          if (currentFloor == destination) {
            // play the sound the elevator make when it arrives to the destination
            tone(buzzerPin, dingTone, dingDelay);
            dingTime = millis();
          }
          else {
            // play the elevator moving sound
            tone(buzzerPin, elevatorMovingTone);
          }
        }
      }
    }
  }
  else {
    elevatorLedState = HIGH;
  }
  
  for (int i = 0; i < 3; i++) {
    if (buttons[i].pressed()) {
      // if the elevator is not moving you can go to any floor except for the current one
      
      if (!elevatorMoving && i != currentFloor) {
        memory.push(i);
      }
      // if the elevator is moving you can go to any floor exept for the one that is first in the queue or the current destination
      else if (elevatorMoving && !memory.isEmpty() && i != memory.getRear()) {
        memory.push(i);
      } 
      else if (elevatorMoving && memory.isEmpty() && i != destination) {
        memory.push(i);
      }
    }
    
    digitalWrite(floorLedPins[i], floorLedStates[i]);
  }

  // wait for the "ding" sound to be over before going to another floor
  if (currentTime - dingTime > dingDelay + 100) {
    if (!memory.isEmpty() && !elevatorMoving) {
      destination = memory.pop();

      elevatorMoving = true;
      
      // play the elevator doors closing sound
      tone(buzzerPin, doorsClosingTone, doorsClosingDelay);
      doorsClosingTime = millis();

      previousBlinkTime = millis();
      previousChangeFloorTime = millis();
      previousOnFloorTime = millis();
    }
  }
  digitalWrite(elevatorLedPin, elevatorLedState);
}
