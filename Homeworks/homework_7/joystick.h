#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "objects.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SHORT_PRESS 1
#define LONG_PRESS 2

// Check if the joystick moved and update the position of the player
void readJoystickMovement(Player& player, int pinX, int pinY);

int readJoystickMovementMenu(int pinX, int pinY, int& currentOption, int numOptions, LiquidCrystal lcd);

byte setLcdBrightnessWithJoystick(int pinX, int pinY, byte contrast, LiquidCrystal lcd);

byte setMatrixBrightnessWithJoystick(int pinX, int pinY, byte matrixBrightness, LiquidCrystal lcd);

// Check if the button was pressed and return if it was a short or a long press
int joystickButtonPress(int pinSW);

#endif