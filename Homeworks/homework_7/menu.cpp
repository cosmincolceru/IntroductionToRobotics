#import "menu.h"

String options[4] = {
  "Start game",
  "Settings",
  "About",
  ""
};

void printMenu(LiquidCrystal lcd, int currentOption) {
  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(options[currentOption]);
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.print(options[currentOption + 1]);
}

String settings[4] = {
  "LCD",
  "Matrix",
  "Back",
  ""
};

void printSettingsMenu(LiquidCrystal lcd, int currentOption) {
  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(settings[currentOption]);
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.print(settings[currentOption + 1]);
}

String about[8] = {
  "Break the walls",
  "Author: Cosmin ",
  "Colceru", 
  "GitHub: https://",
  "github.com/cosmi",
  "ncolceru/Introdu",
  "ctionToRobotics/"
  ""
};

void printAboutMenu(LiquidCrystal lcd, int currentOption){
  lcd.setCursor(0, 0);
  lcd.print(about[currentOption]);
  lcd.setCursor(0, 1);
  lcd.print(about[currentOption + 1]);
}
