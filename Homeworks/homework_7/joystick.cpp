#include "LiquidCrystal.h"
#include "joystick.h"

// Potentiometers variables
int minThreshold = 200;
int maxThreshold = 800;
bool joyMoved = false;

// Button variables
byte lastReading = LOW;
byte swState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastJoyMoved = 0;
unsigned long moveTime = 600;

const int longPressTime = 1000;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

// Check if the joystick moved and update the position of the player
void readJoystickMovement(Player& player, int pinX, int pinY) {
  // Read the values from the potentiometers
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  // check if the joystick moved up, down, left or right
  if (xValue > maxThreshold && joyMoved == false) {
    // Move player 1 position right
    player.direction = RIGHT;

    lastJoyMoved = millis();

    int newY = min(player.y + 1, MATRIX_SIZE - 1);
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

    int newX = min(player.x + 1, MATRIX_SIZE - 1);
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

int readJoystickMovementMenu(int pinX, int pinY, int& currentOption, int numOptions, LiquidCrystal lcd) {
  // Read the values from the potentiometers
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);
 
  if (yValue < minThreshold && joyMoved == false) {
    lcd.clear();
    currentOption = min(numOptions - 1, currentOption + 1);    
    joyMoved = true;
  }
  if (yValue > maxThreshold && joyMoved == false) {
    lcd.clear();
    currentOption = max(0, currentOption - 1);
    
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && 
      yValue >= minThreshold && yValue <= maxThreshold) {
    
    joyMoved = false;
  }

  return -1;
}

byte setLcdBrightnessWithJoystick(int pinX, int pinY, byte contrast, LiquidCrystal lcd) {
  // Read the values from the potentiometers
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMoved == false) {
    contrast = max(0, contrast - 10);

    EEPROM.put(0, contrast);
    joyMoved = true;
    lcd.clear();
  }
  if (yValue > maxThreshold && joyMoved == false) {
    contrast = min(200, contrast + 10);
    EEPROM.put(0, contrast);
    joyMoved = true;
    lcd.clear();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && 
      yValue >= minThreshold && yValue <= maxThreshold) {
    
    joyMoved = false;
  }
  
  return contrast;
}

byte setMatrixBrightnessWithJoystick(int pinX, int pinY, byte matrixBrightness, LiquidCrystal lcd) {
  // Read the values from the potentiometers
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMoved == false) {
      matrixBrightness = max(0, matrixBrightness - 1);
    EEPROM.put(2, matrixBrightness);
    joyMoved = true;
    lcd.clear();
  }
  if (yValue > maxThreshold && joyMoved == false) {
    matrixBrightness = min(15, matrixBrightness + 1);
    EEPROM.put(2, matrixBrightness);
    joyMoved = true;
    lcd.clear();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && 
      yValue >= minThreshold && yValue <= maxThreshold) {
    
    joyMoved = false;
  }
  
  return matrixBrightness;
}


// Check if the button was pressed and return if it was a short or a long press
int joystickButtonPress(int pinSW) {
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
