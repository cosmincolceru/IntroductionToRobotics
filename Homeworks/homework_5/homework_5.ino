#include <EEPROM.h>

// Ultrasonic sensor variables
const byte trigPin = 9;
const byte echoPin = 10;
long duration = 0;
int distance = 0;
byte ultrasonicSamplingInterval;
byte ultrasonicThreshold = 10;
bool ultrasonicIntervalReading = false;
bool setUltrasonicThreshold = false;
unsigned long lastUltrasonicReading = 0;
bool ultrasonicAlert = false;
const byte minUltrasonicValue = 2;
const byte maxUltrasonicValue = 100;

// Photoresistor variables
int photocellPin = 0;
int photocellValue;    
byte ldrSamplingInterval;
int ldrThreshold = 200;
bool ldrIntervalReading = false;
unsigned long lastLdrReading = 0;
bool setLdrThreshold = false;
bool ldrAlert = false;
const int minLdrValue = 0;
const int maxLdrValue = 1023;

const int minSampleInterval = 1;
const int maxSampleInterval = 10;

// buzzer variables
const int buzzerPin = 11;
const int alertTone = 1500;
const int alertDuration = 500;
unsigned long lastAlertTime = 0;

// Led variables
const byte redPin = 3;
const byte greenPin = 5;
const byte bluePin = 6;

byte redValue = 0;
byte greenValue = 255;
byte blueValue = 0;

bool printSensorReadings = false;
unsigned long lastSensorsPrint = 0;

bool manualColorControlMode = false;
bool automaticMode = true;

// LED color for alert on automatic mode
const byte autoRedValueAlert = 255;
const byte autoGreenValueAlert = 0;
const byte autoBlueValueAlert = 0;

// LED normal color
const byte redValueNormal = 0;
const byte greenValueNormal = 255;
const byte blueValueNormal = 0;

// LED color for alert on manual mode
byte manualRedValueAlert = 255;
byte manualGreenValueAlert = 0;
byte manualBlueValueAlert = 0;

bool redSet = false;
bool greenSet = false;
bool blueSet = false;

const int noOfSavedReadings = 10;
int savedUltrasonicReadings[noOfSavedReadings];
int savedLdrReadings[noOfSavedReadings];
byte savedUltrasonicIndex = 0;
byte savedLdrIndex = 0;

int currentMenu = 0;
// Main Menu                  - 0
// Sensor Settings Menu       - 1
// Reset Logger Menu          - 2
// System Settings Menu       - 3
// RGB LED Control Menu       - 4
// Sampling Interval Menu     - 5
// Yes/No Menu for ultrasonic - 6
// Yes/No Menu for ldr        - 7

/*
  EEPROM Structure
  ----------------
  Last 10 ultrasonic readings         - int
  Last 10 LDR readings                - int      
  Ultrasonic sensor sampling interval - byte
  LDR sensor sampling interval        - byte
  Ultrasonic sensor threshold         - byte
  LDR sensor threshold                - int
  Manual LED red                      - byte
  Manual LED green                    - byte
  Manual LED blue                     - byte
  Automatic Mode                      - bool
  Saved ultrasonic index              - byte
  Saved ldr index                     - byte
*/

// Calculate the EEPROM adreeses for each stored value
int ultrasonicAddress = 0;
int ldrAddress = ultrasonicAddress + sizeof(savedUltrasonicReadings);
int ultrasonicSamplingAddress = ldrAddress + sizeof(savedLdrReadings);
int ldrSamplingAddress = ultrasonicSamplingAddress + sizeof(ultrasonicSamplingInterval);
int ultrasonicThresholdAddress = ldrSamplingAddress + sizeof(ldrSamplingInterval);
int ldrThresholdAddress = ultrasonicThresholdAddress + sizeof(ultrasonicThreshold);
int redAddress = ldrThresholdAddress + sizeof(ldrThreshold);
int greenAddress = redAddress + sizeof(manualRedValueAlert);
int blueAddress = greenAddress + sizeof(manualGreenValueAlert);
int automaticModeAdress = blueAddress + sizeof(manualBlueValueAlert);
int ultrasonicIndexAddress = automaticModeAdress + sizeof(automaticMode);
int ldrIndexAddress = ultrasonicIndexAddress + sizeof(savedUltrasonicIndex);

void setup() {
  pinMode(photocellPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  
  // Read the saved settings from EEPROM
  EEPROM.get(ultrasonicSamplingAddress, ultrasonicSamplingInterval);
  EEPROM.get(ldrSamplingAddress, ldrSamplingInterval);
  EEPROM.get(ultrasonicThresholdAddress, ultrasonicThreshold);
  EEPROM.get(ldrThresholdAddress, ldrThreshold);

  EEPROM.get(redAddress, manualRedValueAlert);
  EEPROM.get(greenAddress, manualGreenValueAlert);
  EEPROM.get(blueAddress, manualBlueValueAlert);

  EEPROM.get(automaticModeAdress, automaticMode);

  printMainMenu();
}

void loop() {
  // Read light value
  if (millis() - lastLdrReading > 1000 * ldrSamplingInterval) {
      photocellValue = analogRead(photocellPin);
      lastLdrReading = millis();
  
      ldrAlert = photocellValue < ldrThreshold;

      EEPROM.get(ldrIndexAddress, savedLdrIndex); // Get current index
      // Serial.print("savedLdrIndex: ");
      // Serial.println(savedLdrIndex);
      savedLdrReadings[savedLdrIndex] = photocellValue; 
      EEPROM.put(ldrAddress + savedLdrIndex * sizeof(photocellValue), photocellValue); // Save the value
      savedLdrIndex = (savedLdrIndex + 1) % noOfSavedReadings; // Update the index
      EEPROM.put(ldrIndexAddress, savedLdrIndex);  // Save the index
  }

  // Read ultrasonic value
  if (millis() - lastUltrasonicReading > 1000 * ultrasonicSamplingInterval) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    lastUltrasonicReading = millis();

    ultrasonicAlert = distance < ultrasonicThreshold;
 
    EEPROM.get(ultrasonicIndexAddress, savedUltrasonicIndex); // Get current index
    // Serial.print("savedUltrasonicIndex: ");
    // Serial.println(savedUltrasonicIndex);
    savedUltrasonicReadings[savedUltrasonicIndex] = distance; 
    EEPROM.put(ultrasonicAddress + savedUltrasonicIndex * sizeof(distance), distance); // Save the value
   
    savedUltrasonicIndex = (savedUltrasonicIndex + 1) % noOfSavedReadings; // Update the index
    EEPROM.put(ultrasonicIndexAddress, savedUltrasonicIndex);  // Save the index
  }

  if (printSensorReadings) {
    if (millis() - lastSensorsPrint >= 1000 * min(ultrasonicSamplingInterval, ldrSamplingInterval)) {
      Serial.print("Ultrasonic: ");
      Serial.print(distance);
      Serial.print("  LDR: ");
      Serial.println(photocellValue);

      lastSensorsPrint = millis();
    }
  }

  if (ldrAlert || ultrasonicAlert) {
    if (millis() - lastAlertTime < alertDuration) {
      tone(buzzerPin, alertTone);
    }
    else {
      noTone(buzzerPin);
    }
    if (automaticMode) {
      redValue = autoRedValueAlert;
      greenValue = autoGreenValueAlert;
      blueValue = autoBlueValueAlert;
    }
    else {
      redValue = manualRedValueAlert;
      greenValue = manualGreenValueAlert;
      blueValue = manualBlueValueAlert;
    }
  }
  else {
    lastAlertTime = millis();
    redValue = redValueNormal;
    greenValue = greenValueNormal;
    blueValue = blueValueNormal;
  }

  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);

  if (Serial.available() > 0) {
    String rawInput = Serial.readString();
    int input = 0;
    
    // printSensorReadings
    if (rawInput == "q") {
      input = -2;
    }
    else {
      for (int i = 0; i < rawInput.length(); i++) {
        if (rawInput[i] >= '0' && rawInput[i] <= '9') {
          input = input * 10 + (rawInput[i] - '0');
        }
        else {
          input = -1;
          break;
        }
      }
    }
    
    // Serial.println(printSensorReadings);

    if (manualColorControlMode) {
      // Set the RGB LED color for manual mode
      if (input >= 0 && input <= 255) {
        if (!redSet) {
          // Set red value
          manualRedValueAlert = input;
          redSet = true;
          EEPROM.put(redAddress, manualRedValueAlert);
          Serial.print(F("    Red value set to "));
          Serial.println(input);
          Serial.println(F("    Input value for green (0-255): "));
        }
        else if (!greenSet) {
          // Set green value
          manualGreenValueAlert = input;
          greenSet = true;
          EEPROM.put(greenAddress, manualGreenValueAlert);
          Serial.print(F("    Green value set to "));
          Serial.println(input);
          Serial.println(F("    Input value for blue (0-255): "));
        }
        else {
          // Set blue value
          manualBlueValueAlert = input;
          EEPROM.put(blueAddress, manualBlueValueAlert);
          manualColorControlMode = false;

          Serial.print(F("    Blue value set to "));
          Serial.println(input);
          printMainMenu();
          currentMenu = 0;
        }
      }
      else {
        if (!redSet) {
          Serial.println(F("    Invalid input. Try again."));
          Serial.println(F("    Input value for red (0-255): "));
        }
        else if (!greenSet) {
          Serial.println(F("    Invalid input. Try again."));
          Serial.println(F("    Input value for green (0-255): "));
        }
        else {
          Serial.println(F("    Invalid input. Try again."));
          Serial.println(F("    Input value for blue (0-255): "));
        }
      }

    }
    else if (printSensorReadings && input == -2) {
      // Turn off printing current sensor values
      printSensorReadings = false;
      currentMenu = 0;
      printMainMenu();
    }
    else if (ldrIntervalReading) {
      // Use the input as sampling interval for the LDR sensor
      if (input >= minSampleInterval && input <= maxSampleInterval) {
        ldrSamplingInterval = input;
        EEPROM.put(ldrSamplingAddress, ldrSamplingInterval);
        Serial.println(F("======================================="));
        Serial.print(F("Sampling interval for the LDR sensor set to "));
        Serial.print(input);
        Serial.println(F(" seconds"));
        currentMenu = 0;
        printMainMenu();
        ldrIntervalReading = false;
      }
      else {
        Serial.println(F("======================================="));
        Serial.println(F("      Invalid input. Try again."));
        Serial.println(F("      Set sampling interval for the ultrasonic sensor (1 - 10 seconds): "));
      }
    }
    else if (ultrasonicIntervalReading) {
      // Use the input as sampling interval for the ultrasonic sensor
        if (input >= minSampleInterval && input <= maxSampleInterval) {
          ultrasonicSamplingInterval = input;
          EEPROM.put(ultrasonicSamplingAddress, ultrasonicSamplingInterval);
          Serial.println(F("======================================="));
          Serial.print(F("Sampling interval for the ultrasonic sensor set to "));
          Serial.print(input);
          Serial.println(F(" seconds"));
          currentMenu = 0;
          printMainMenu();
          ultrasonicIntervalReading = false;
      }
      else {
        Serial.println(F("======================================="));
        Serial.println(F("      Invalid input. Try again."));
        Serial.println(F("      Set sampling interval for the ultrasonic sensor (1 - 10 seconds): "));
      }
    }
    else if (setLdrThreshold) {
      // Use the input as threshold for the LDR sensor
      if (input >= minLdrValue && input <= maxLdrValue) {
        ldrThreshold = input;
        EEPROM.put(ldrThresholdAddress, ldrThreshold);
        Serial.println(F("======================================="));
        Serial.print(F("Threshold value LDR sensor set to "));
        Serial.println(input);
        currentMenu = 0;
        printMainMenu();

        setLdrThreshold = false;
      }
      else {
        Serial.println(F("======================================="));
        Serial.println(F("    Invalid input. Try again."));
        Serial.println(F("    Set threshold for the LDR sensor (0 - 1023): "));
      }
    }
    else if (setUltrasonicThreshold) {
      // Use the input as threshold for the ultrasonic sensor
      if (input >= minUltrasonicValue && input <= maxUltrasonicValue) {
        ultrasonicThreshold = input;
        EEPROM.put(ultrasonicThresholdAddress, ultrasonicThreshold);
        Serial.println(F("======================================="));
        Serial.print(F("Threshold value ultrasonic sensor set to "));
        Serial.print(input);
        Serial.println(F(" cm"));
        currentMenu = 0;
        printMainMenu();

        setUltrasonicThreshold = false;
      }
      else {
        Serial.println(F("======================================="));
        Serial.println(F("    Invalid input. Try again."));
        Serial.println(F("    Set threshold for the ultrasonic sensor (3 - 100 cm): "));
      }
    }
    else {
      switch (currentMenu) {
        case 0: {
          // Main Menu
          switch (input) {
            case 1: {
              printSensorSettingsMenu();
              currentMenu = 1;
              break;
            }
            case 2: {
              printResetLoggerMenu();
              currentMenu = 2;
              break;
            }
            case 3: {
              printSystemStatusMenu();
              currentMenu = 3;
              break;
            }
            case 4: {
              printRGBMenu();
              currentMenu = 4;
              break;
            }
            default: {
              Serial.println(F("======================================="));
              Serial.println(F("Invalid option. Try again."));
              printMainMenu();
              currentMenu = 0;
              break;
            }
          }
          break;
          // End Main Menu
        }

        case 1: {
          // Sensor Settings Menu
          switch (input) {
            case 1: {
              printSamplingMenu();
              currentMenu = 5;
              break;
            }
            case 2: {
              Serial.println(F("======================================="));
              Serial.println(F("    Set threshold for the ultrasonic sensor (3 - 100 cm): "));

              // read the ultrasonic threshold
              setUltrasonicThreshold = true;
              break;
            }
            case 3: {
              Serial.println(F("======================================="));
              Serial.println(F("    Set threshold for the LDR sensor (0 - 1023): "));

              // read the LDR threshold
              setLdrThreshold = true;
              break;
            }
            case 4: {
              printMainMenu();
              currentMenu = 0;
              break;
            }
            default: {
              Serial.println(F("======================================="));
              Serial.println(F("Invalid option. Try again."));
              printSensorSettingsMenu();
              currentMenu = 1;
              break;
            }
          }
          break;
          // End Sensor Settings Menu
        }

        case 2: {
          // Sensor Settings Menu 

          switch (input) {
            case 1: {
              // Reset Ultrasonic Sensor Data 
              printYesNoUltrasonic();

              currentMenu = 6;
              break;
            }
            case 2: {
              // Reset LDR Sensor Data
              printYesNoLdr();
              currentMenu = 7;
              break;
            }
            case 3: {
              // Back
              printMainMenu();
              currentMenu = 0;
              break;
            }
            default: {
              Serial.println(F("======================================="));
              Serial.println(F("Invalid option. Try again."));
              printResetLoggerMenu();
              currentMenu = 2;
              break;
            }
          }

          // End Sensor Settings Menu 
          break;
        }

        case 3: {
          // System Status Menu
          switch (input) {
            case 1: {
              // Current Sensor Readings
              Serial.println(F("Current Sensor Readings: "));
              printSensorReadings = true;
              
              break;
            }
            case 2: {
              // Current Sensor Settings
              Serial.println(F("======================================="));
              Serial.println(F("Current Sensor Settings: "));
              Serial.println(F("Ultrasonic Sensor: "));
              Serial.print(F("  - Sampling interval: "));
              Serial.print(ultrasonicSamplingInterval);
              Serial.println(F(" seconds"));
              Serial.print(F("  - Threshold: "));
              Serial.println(ultrasonicThreshold);
              Serial.println(F("LDR Sensor: "));
              Serial.print(F("  - Sampling interval: "));
              Serial.print(ldrSamplingInterval);
              Serial.println(F(" seconds"));
              Serial.print(F("  - Threshold: "));
              Serial.println(ldrThreshold);

              printMainMenu();
              currentMenu = 0;
              break;
            }
            case 3: {
              // Display Logged Data
              Serial.println(F("======================================="));
              Serial.println(F("Logged Data: "));
              Serial.print(F("Last "));
              Serial.print(noOfSavedReadings);
              Serial.println(F(" readings from the ultrasonic sensor: "));
              
              byte currentIndex;
              EEPROM.get(ultrasonicIndexAddress, currentIndex);
              
              int currentIndexI = currentIndex;
              currentIndexI -= 1;

              for (int i = currentIndexI; i >= 0; i--) {
                int value;
                EEPROM.get(ultrasonicAddress + i * sizeof(value), value);
                Serial.print(value);
                Serial.print(F("; "));
              }
              for (int i = noOfSavedReadings - 1; i > currentIndexI; i--) {
                int value;
                EEPROM.get(ultrasonicAddress + i * sizeof(value), value);
                Serial.print(value);
                Serial.print(F("; "));
              }

              Serial.print(F("\nLast "));
              Serial.print(noOfSavedReadings);
              Serial.println(F(" readings from the LDR sensor: "));
              EEPROM.get(ldrIndexAddress, currentIndex);

              currentIndexI = currentIndex;
              currentIndexI -= 1;

              for (int i = currentIndexI; i >= 0; i--) {
                int value;
                EEPROM.get(ldrAddress + i * sizeof(value), value);
                Serial.print(value);
                Serial.print(F("; "));
              }
              for (int i = noOfSavedReadings - 1; i > currentIndexI; i--) {
                int value;
                EEPROM.get(ldrAddress + i * sizeof(value), value);
                Serial.print(value);
                Serial.print(F("; "));
              }
              Serial.println();
              printMainMenu();
              currentMenu = 0;
              break;
            }
            case 4: {
              // Back
              printMainMenu();
              currentMenu = 0;
              break;
            }
            default: {
                Serial.println(F("======================================="));
                Serial.println(F("Invalid option. Try again."));
                printSystemStatusMenu();
                currentMenu = 3;
                break;
            }
          }

          // End System Status Menu
          break;
        }

        case 4: {
          // RGB LED Control Menu
          switch (input) {
            case 1: {
              // Manual Color Control
              Serial.println(F("======================================="));
              Serial.println(F("    Manual Color Control"));
              Serial.println(F("    Input value for red (0-255): "));
              manualColorControlMode = true;
              redSet = false;
              greenSet = false;
              blueSet = false;
              break;
            }
            case 2: {
              // LED: Toggle Automatic ON/OFF

              automaticMode = !automaticMode;
              EEPROM.put(automaticModeAdress, automaticMode);
              // Serial.println("    LED: Toggle Automatic ON/OFF");
              Serial.println(F("======================================="));
              Serial.print(F("Automatic mode set to: "));
              if (automaticMode) {
                Serial.println(F("ON"));
              }
              else {
                Serial.println(F("OFF"));
              }
              
              printMainMenu();
              currentMenu = 0;
              break;
            }
            case 3: {
              // Back
              printMainMenu();
              currentMenu = 0;
              break;
            }
            default: {
              Serial.println(F("======================================="));
              Serial.println(F("Invalid option. Try again."));
              printRGBMenu();
              currentMenu = 4;
              break;
            }
          }

          break;
          // End RGB LED Control Menu
        }

        case 5: {
          // Sampling Interval Menu
          switch (input) {
            case 1: {
              Serial.println(F("======================================="));
              Serial.println(F("      Set sampling interval for the ultrasonic sensor (1-10 seconds): "));

              // read the ultrasonic interval
              ultrasonicIntervalReading = true;
              break;
            }
            case 2: {
              Serial.println(F("======================================="));
              Serial.println(F("      Set sampling interval for the LDR sensor (1-10 seconds): "));

              // // read the LDR interval0
              ldrIntervalReading = true;
              break;
            }
            case 3: {
              printSensorSettingsMenu();
              currentMenu = 1;
              break;
            }
            default: {
              Serial.println(F("======================================="));
              Serial.println(F("Invalid option. Try again."));
              printSamplingMenu();
              currentMenu = 5;
              break;
            }
          }

          // End Sampling Interval Menu
          break;
        }
        case 6: {
          // Yes/No Menu for ultrasonic sensor
          switch (input) {
            case 1: {
              // Delete data
              for (int i = 0; i < noOfSavedReadings; i++) {
                EEPROM.put(ultrasonicAddress + i * sizeof(int), 0);
              }
              Serial.println(F("======================================="));
              Serial.println(F("Logger data for ultrasonic sensor deleted."));
              printMainMenu();
              currentMenu = 0;
              break;
            }
            case 2: {
              printMainMenu();
              currentMenu = 0;
              break;
            } 
            default: {
              Serial.println(F("  ======================================="));
              Serial.println(F("  Invalid option. Try again."));
              printYesNoUltrasonic();
              currentMenu = 6;
              break;
            }
          }
          
          break;
        }
        case 7: {
          // Yes/No Menu for ldr sensor
          switch (input) {
            case 1: {
              // Delete data
              for (int i = 0; i < noOfSavedReadings; i++) {
                EEPROM.put(ldrAddress + i * sizeof(int), 0);
              }
              Serial.println(F("======================================="));
              Serial.println(F("Logger data for LDR sensor deleted."));
              printMainMenu();
              currentMenu = 0;
              break;
            }
            case 2: {
              printMainMenu();
              currentMenu = 0;
              break;
            } 
            default: {
              Serial.println(F("  ======================================="));
              Serial.println(F("  Invalid option. Try again."));
              printYesNoUltrasonic();
              currentMenu = 7;
              break;
            }
          }
          break;
        }
      }
    }
  }  
}

void printMainMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("Main Menu:"));
  Serial.println(F("1. Sensor Settings"));
  Serial.println(F("2. Reset Logger Data"));
  Serial.println(F("3. System Status"));
  Serial.println(F("4. RGB LED Control"));
}

void printSensorSettingsMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("  Sensor Settings Menu:"));
  Serial.println(F("  1. Sensors Sampling Interval"));
  Serial.println(F("  2. Ultrasonic Alert Threshold"));
  Serial.println(F("  3. LDR Alert Threshold"));
  Serial.println(F("  4. Back"));
}

void printResetLoggerMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("  Reset Logger Data Menu:"));
  Serial.println(F("  1. Reset Ultrasonic Sensor Data"));
  Serial.println(F("  2. Reset LDR Sensor Data"));
  Serial.println(F("  3. Back"));
}

void printYesNoLdr() {
  Serial.println(F("======================================="));
  Serial.println(F("    Are you sure you want to delete the LDR sensor data?"));
  Serial.println(F("    1. Yes"));
  Serial.println(F("    2. No"));
}

void printYesNoUltrasonic() {
  Serial.println(F("======================================="));
  Serial.println(F("    Are you sure you want to delete the ultrasonic sensor data?"));
  Serial.println(F("    1. Yes"));
  Serial.println(F("    2. No"));
}

void printSystemStatusMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("  System Status Menu"));
  Serial.println(F("  1. Current Sensor Readings (type q to quit)"));
  Serial.println(F("  2. Current Sensor Settings"));
  Serial.println(F("  3. Display Logged Data"));
  Serial.println(F("  4. Back"));
}

void printRGBMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("  RGB LED Control Menu"));
  Serial.println(F("  1. Manual Color Control"));
  Serial.print(F("  2. LED: Toggle Automatic "));
  if (!automaticMode) {
    Serial.println(F("ON"));
  }
  else {
    Serial.println(F("OFF"));
  }
  // Serial.println("  2. LED: Toggle Automatic ON/OFF");
  Serial.println(F("  3. Back"));
}

void printSamplingMenu() {
  Serial.println(F("======================================="));
  Serial.println(F("    Set sampling interval for: "));
  Serial.println(F("    1. Ultrasonic Sensor"));
  Serial.println(F("    2. LDR Sensor"));
  Serial.println(F("    3. Back"));
}
