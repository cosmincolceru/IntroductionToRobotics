#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h" 
#include "matrix.h"
#include "objects.h"
#include "joystick.h"
#include "menu.h"

// Matrix driver pins
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

// Joystick pins
const int pinSW = 2; 
const int pinX = A0;
const int pinY = A1;

// LCD pins
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte contrastPin = 3;

byte contrast;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); 
Player player;
Bullet bullets[MAX_BULLETS];

byte matrixBrightness = 2;

int numWalls;

unsigned long lastIncrement = 0;
unsigned long delayCount = 1000;  // Delay between updates (milliseconds)
unsigned long seconds = 0;
unsigned long minutes = 0;

int introMessageTime = 2000;
unsigned long elapsedIntroTime = 0;

byte upArrow[8] = {
  0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
}; 
byte downArrow[8] = {
  0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100
};

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(5));

  // Initiate joystick pins
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  // Initiate the matrix
  lc.shutdown(0, false);                 
  EEPROM.get(2, matrixBrightness);
  lc.setIntensity(0, matrixBrightness);  
  lc.clearDisplay(0);  

  // Initiate the LCD
  pinMode(contrastPin, OUTPUT);
  EEPROM.get(0, contrast);
  analogWrite(contrastPin, contrast);
  lcd.begin(16, 2);

  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);
}

bool displayIntro = true;
bool menuPrinted = false;
bool gameStarted = false;
bool levelInitiated = false;
bool gameEnded = false;
bool printAbout = false;
bool displaySettings = false;
bool setLcdBrightness = false;
bool setMatrixBrightness = false;

int currentOption = 0;
 
void loop() {
  // Display the welcome message
  if (elapsedIntroTime > introMessageTime) {
    displayIntro = false;    
  } else {
    elapsedIntroTime = millis();
  }
  if (displayIntro) {
    lcd.setCursor(0, 0);
    lcd.print("Welcome!");
  }
  else {
    if (!gameStarted && !gameEnded && !printAbout && !displaySettings && !setLcdBrightness && !setMatrixBrightness) {
      // Print the menu and move through the options
      printMenu(lcd, currentOption);
      readJoystickMovementMenu(pinX, pinY, currentOption, 3, lcd);

      if (joystickButtonPress(pinSW)) {
        switch (currentOption) {
          case 0: {
            gameStarted = true;
            levelInitiated = false;
            break;
          }
          case 1: {
            displaySettings = true;
            lcd.clear();
            currentOption = 0;
            break;
          }
          case 2: {
            printAbout = true;
            currentOption = 0;
            lcd.clear();
            break;
          }
        }
      }
    }



    if (displaySettings) {
      readJoystickMovementMenu(pinX, pinY, currentOption, 3, lcd);
      printSettingsMenu(lcd, currentOption);

      if (joystickButtonPress(pinSW)) {
        switch (currentOption) {
          case 0: {
            setLcdBrightness = true;
            displaySettings = false;
            lcd.clear();
            break;
          }
          case 1: {
            setMatrixBrightness = true;
            displaySettings = false;
            lcd.clear();
            break;
          }
          case 2: {
            displaySettings = false;
            currentOption = 0;
            lcd.clear();
          }
        }
      }
    }

    if (setLcdBrightness) {
      lcd.setCursor(0, 0);
      lcd.print("Brigtness");
      lcd.setCursor(15, 0);
      lcd.write((byte)0);
      lcd.setCursor(15, 1);
      lcd.write((byte)1);

      contrast = setLcdBrightnessWithJoystick(pinX, pinY, contrast, lcd);
      analogWrite(contrastPin, contrast);
      lcd.setCursor(0, 1);
      lcd.print(contrast);

      if (joystickButtonPress(pinSW)) {
        setLcdBrightness = false;
        lcd.clear();
      }
    }

    if (setMatrixBrightness) {
      lcd.setCursor(15, 0);
      lcd.write((byte)0);
      lcd.setCursor(15, 1);
      lcd.write((byte)1);
      
      matrixBrightness = setMatrixBrightnessWithJoystick(pinX, pinY, matrixBrightness, lcd);
      lc.setIntensity(0, matrixBrightness);  
      lcd.setCursor(0, 0);
      lcd.print(matrixBrightness);

      matrixOn(matrix);
      updateMatrix(lc, matrix);

      if (joystickButtonPress(pinSW)) {
        setMatrixBrightness = false;
        lcd.clear();
        clearMatrix(matrix);
        updateMatrix(lc, matrix);
      }
    }

    if (printAbout) {
      readJoystickMovementMenu(pinX, pinY, currentOption, 7, lcd);
      printAboutMenu(lcd, currentOption);

      if (joystickButtonPress(pinSW)) {
        printAbout = false;
        currentOption = 0;
        lcd.clear();
      }
    }

    if (gameStarted && !levelInitiated) {
      // initiate the player and the walls
      player = generatePlayer();
      numWalls = generateWalls(matrix, player);

      seconds = 0;
      minutes = 0;
      lastIncrement = millis();

      lcd.clear();
      lcd.print("Walls: ");
      lcd.print(numWalls);
      levelInitiated = true;
    }

    if (gameStarted && levelInitiated) {
      bilnkPlayer(player);
 
      // Move the player
      readJoystickMovement(player, pinX, pinY);

      // Handle button press
      int currentButtonState = joystickButtonPress(pinSW);
      if (currentButtonState == SHORT_PRESS) {
        // Start the bullet at the player's position
        shootBullet(player, bullets);
      }
      else if (currentButtonState == LONG_PRESS) {
        // Clear the matrix and generate new level
        clearMatrix(matrix);
        player = generatePlayer();
        numWalls = generateWalls(matrix, player);

        seconds = 0;
        minutes = 0;
        
        lcd.setCursor(0, 0);
        lcd.print("Walls: ");
        lcd.print(numWalls);
      }

      // Display the bullets
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].bulletShot == true) {
          moveBullet(bullets, i, player, numWalls);
          lcd.setCursor(0, 0);
          
          lcd.print("Walls: ");
          lcd.print(numWalls);
          if (numWalls < 10) {
          lcd.print(" "); 
          }
        }
      }

      // While there are still walls increment the time
      if (numWalls > 0) {
        if (millis() - lastIncrement > delayCount) {
          seconds++;
          if (seconds == 60) {
            seconds = 0;
            minutes++;
          }
          lastIncrement = millis();
        }

        lcd.setCursor(0, 1);
        lcd.print("Time: ");
        if (minutes < 10) {
          lcd.print("0");
        }
        lcd.print(minutes);
        lcd.print(":");
        if (seconds < 10) {
          lcd.print("0");
        }
        lcd.print(seconds);  
      }
      else {
        // Print the end game message
        gameEnded = true;
        gameStarted = false;
        clearMatrix(matrix
        );
        lcd.clear();        
      }
      
      updateMatrix(lc, matrix);
    }

    if (gameEnded) {
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      if (minutes < 10) {
        lcd.print("0");
      }
      lcd.print(minutes);
      lcd.print(":");
      if (seconds < 10) {
        lcd.print("0");
      }
      lcd.print(seconds);  
      lcd.setCursor(0, 1);
      lcd.print("Return to menu");

      // Return to menu if the button is pressed
      if (joystickButtonPress(pinSW)) {
        gameEnded = false;
        lcd.clear();
      }
    } 
  }
}
