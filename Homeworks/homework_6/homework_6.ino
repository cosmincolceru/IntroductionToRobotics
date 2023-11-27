#include "LedControl.h"  

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define SHORT_PRESS 1
#define LONG_PRESS 2

// Matrix driver pins
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

const byte matrixSize = 8;
byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

// joystick pins
const int pinSW = 2; 
const int pinX = A0;
const int pinY = A1;

// Potentiometers variables
int xValue = 0;
int yValue = 0;
int minThreshold = 200;
int maxThreshold = 800;
bool joyMoved = false;

// Button variables
byte lastReading = LOW;
byte swState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastPlayerBlink;
unsigned long playerBlinkTime = 300;

const int minWallsPercentage = 50;
const int maxWallsPercentage = 75;

unsigned long lastJoyMoved = 0;
unsigned long moveTime = 600;

const int longPressTime = 1000;

unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

struct Player {
  byte x, y;
  byte direction;
} player;

struct Bullet {
  byte x, y;
  byte direction;
  bool bulletShot;
  unsigned long lastBulletBlink = 0;
  unsigned long lastBulletMove = 0;
};

unsigned long bulletBlinkTime = 100;
unsigned long bulletMoveTime = 200;

const int maxNoOfBullets = 5;
Bullet bullets[maxNoOfBullets];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(5));

  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  lc.shutdown(0, false);                 
  lc.setIntensity(0, matrixBrightness);  
  lc.clearDisplay(0);                    

  introAnimation(matrix);

  // Generate player and walls
  player = generatePlayer();
  generateWalls(matrix, player);
  for (int i = 0; i < maxNoOfBullets; i++) {
    bullets[i].x = 0;
    bullets[i].y = 0;
    bullets[i].direction = 0;
    bullets[i].bulletShot = false;
  }
}

void loop() {
  // Blink the player
  if (millis() - lastPlayerBlink > playerBlinkTime) {
    matrix[player.x][player.y] = !matrix[player.x][player.y];
    lastPlayerBlink = millis();
  }

  readJoystickMovement();

  int currentButtonState = joystickButtonPress();
  if (currentButtonState == SHORT_PRESS) {
    // Start the bullet at the player's posiotion
    for (int i = 0; i < maxNoOfBullets; i++) {
      if (bullets[i].bulletShot == false) {
        bullets[i].x = player.x;
        bullets[i].y = player.y;
        bullets[i].direction = player.direction;
        bullets[i].bulletShot = true;
        
        bullets[i].lastBulletBlink = millis();
        bullets[i].lastBulletMove = millis();
        break;
      } 
    }
  }
  else if (currentButtonState == LONG_PRESS) {
    clearMatrix(matrix);
    player = generatePlayer();
    generateWalls(matrix, player);
  }
  
  
  for (int i = 0; i < maxNoOfBullets; i++) {
    if (bullets[i].bulletShot == true) {
      moveBullet(i);
    }
  }

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void introAnimation(byte matrix[matrixSize][matrixSize]) {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 1);
    }
  }

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 0);
      delay(25);
    }
  }
}

void clearMatrix(byte matrix[matrixSize][matrixSize]) {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      matrix[i][j] = 0;
    }
  }
}

void moveBullet(int index) {
  // Blink the bullet
  if ((millis() - bullets[index].lastBulletBlink > bulletBlinkTime) &&
     !(bullets[index].x == player.x && bullets[index].y == player.y)) {
    
    matrix[bullets[index].x][bullets[index].y] = !matrix[bullets[index].x][bullets[index].y];
    bullets[index].lastBulletBlink = millis();
  }

  // Move the bullet in the correct direction
  if (millis() - bullets[index].lastBulletMove > bulletMoveTime) {
    matrix[bullets[index].x][bullets[index].y] = 0;
    if (bullets[index].direction == UP) {
      bullets[index].x--;
    }
    else if (bullets[index].direction == DOWN) {
      bullets[index].x++;
    }
    else if (bullets[index].direction == LEFT) {
      bullets[index].y--;
    }
    else if (bullets[index].direction == RIGHT)  {
      bullets[index].y++;
    }

    // If the bullet hits a wall, destory the wall
    if (matrix[bullets[index].x][bullets[index].y] == 1) {
      matrix[bullets[index].x][bullets[index].y] = 0;
      bullets[index].bulletShot = false;
    }

    // If the bullet exits the matrix do nothing
    if (bullets[index].x < 0 || bullets[index].x >= matrixSize ||
        bullets[index].y < 0 || bullets[index].y >= matrixSize) {
      
      bullets[index].bulletShot = false;
    }

    bullets[index].lastBulletMove = millis();
  }
}

Player generatePlayer() {
  int x = random(0, matrixSize);
  int y = random(0, matrixSize);

  return Player {x, y, 0};
}

void generateWalls(byte matrix[matrixSize][matrixSize], Player player) {
  // Calculate the minimum or maximul number of walls that can be genrated
  int minWalls = (minWallsPercentage * matrixSize * matrixSize) / 100;
  int maxWalls = (maxWallsPercentage * matrixSize * matrixSize) / 100;

  int numWalls = random(minWalls, maxWalls + 1);

  for (int i = 0; i < numWalls; i++) {
    int row = random(0, matrixSize);
    int col = random(0, matrixSize);

    // Don't generate walls over the player
    while (row == player.x && col == player.y) {
      row = random(0, matrixSize);
      col = random(0, matrixSize);
    }
  
    matrix[row][col] = 1;
  }
}

void readJoystickMovement() {
  // Read the values from the potentiometers
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  // check if the joystick moved up, down, left or right
  if (xValue > maxThreshold && joyMoved == false) {
    // Move player 1 position right
    player.direction = RIGHT;

    lastJoyMoved = millis();

    int newY = min(player.y + 1, matrixSize - 1);
    if (matrix[player.x][newY] == 0) {
      matrix[player.x][player.y] = 0; 
      player.y = newY;
    }

    joyMoved = true;
  }
  if (xValue < minThreshold && joyMoved == false) {
    // Move player 1 position left
    player.direction = LEFT;

    lastJoyMoved = millis();
    
    int newY = max(player.y - 1, 0);
    if (matrix[player.x][newY] == 0) {
      matrix[player.x][player.y] = 0; 
      player.y = newY;
    }
    
    joyMoved = true;
  }  
  if (yValue < minThreshold && joyMoved == false) {
    // Move player 1 position down
    player.direction = DOWN;

    lastJoyMoved = millis();

    int newX = min(player.x + 1, matrixSize - 1);
    if (matrix[newX][player.y] == 0) {
      matrix[player.x][player.y] = 0;
      player.x = newX;
    }
    
    joyMoved = true;
  }
  if (yValue > maxThreshold && joyMoved == false) {
    // Move player 1 position up
    player.direction = UP;

    lastJoyMoved = millis();

    int newX = max(player.x - 1, 0);
    if (matrix[newX][player.y] == 0) {
      matrix[player.x][player.y] = 0;
      player.x = newX;
    }
    
    joyMoved = true;
  }

  if (millis() - lastJoyMoved > moveTime) {
    joyMoved = false;
    lastJoyMoved = millis();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && 
      yValue >= minThreshold && yValue <= maxThreshold) {
    
    joyMoved = false;
  }
}

int joystickButtonPress() {
  // Read the button state
  byte reading = !digitalRead(pinSW);

  // Debounce the button and check if the press is long or short
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
          lastReading = reading;
          return SHORT_PRESS;
        }
        else {
          lastReading = reading;
          return LONG_PRESS;
        }
      }
    }
  }

  lastReading = reading;
  return 0;
}

