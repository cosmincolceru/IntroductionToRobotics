// control an RGB LED using 3 potentiometers, one for each channel

const int minAnalogInput = 0;
const int maxAnalogInput = 1023;
const int minAnalogOutput = 0;
const int maxAnalogOutput = 255;

// pins for controlling each channel of the RGB LED
const int redControlPin = A0;
const int greenControlPin = A1;
const int blueControlPin = A2;

const int redLedPin = 6;
const int greenLedPin = 5;
const int blueLedPin = 3;

// values read from the potentiometer
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup() {
  pinMode(redControlPin, INPUT);
  pinMode(greenControlPin, INPUT);
  pinMode(blueControlPin, INPUT);
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

void loop() {
    redValue = analogRead(redControlPin); 
    greenValue = analogRead(greenControlPin);
    blueValue = analogRead(blueControlPin);

    // map the analog input values to the analog output values
    int redLedBrightness = map(redValue, minAnalogInput, maxAnalogInput, minAnalogOutput, maxAnalogOutput);
    int greenLedBrightness = map(greenValue, minAnalogInput, maxAnalogInput, minAnalogOutput, maxAnalogOutput);
    int blueLedBrightness = map(blueValue, minAnalogInput, maxAnalogInput, minAnalogOutput, maxAnalogOutput);

    analogWrite(redLedPin, redLedBrightness);
    analogWrite(greenLedPin, greenLedBrightness);
    analogWrite(blueLedPin, blueLedBrightness);
}