#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>

void printMenu(LiquidCrystal lcd, int currentOption);

void printSettingsMenu(LiquidCrystal lcd, int currentOption);

void printAboutMenu(LiquidCrystal lcd, int currentOption);

#endif