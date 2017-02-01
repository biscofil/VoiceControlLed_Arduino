#include <EEPROM.h>

char c;
String readString;

int leds[] = {10, 6, 5};


byte col[3] = {0, 0, 0};
byte col_x[3] = {0, 0, 0};

unsigned int mode; //0 = random, 1 = serial

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < 3; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i],LOW);
  }

  digitalWrite(leds[0],HIGH);
  delay(1000);
  digitalWrite(leds[0],LOW);
  digitalWrite(leds[1],HIGH);
  delay(1000);
  digitalWrite(leds[1],LOW);
  digitalWrite(leds[2],HIGH);
  delay(1000);
  digitalWrite(leds[2],LOW);

  col[0] = EEPROM.read(0);
  col[1] = EEPROM.read(10);
  col[2] = EEPROM.read(20);

  mode = EEPROM.read(30);
  Serial.println(mode);
  if (mode == 1) {
    EEPROM.write(30,  2);
    Serial.println("AUTO MODE");
  } else {
    EEPROM.write(30,  1);
    Serial.println("SERIAL MODE");
  }

  for (int a = 0; a < 10; a++) {
    analogWrite(leds[0], 0);
    analogWrite(leds[1], 0);
    analogWrite(leds[2], 0);
    delay(100);
    analogWrite(leds[0], (mode == 1) ? 255 : 0);
    analogWrite(leds[1], 0);
    analogWrite(leds[2], (mode == 1) ? 0 : 255);
    delay(100);
  }


  fadeToColor(leds, col_x, col, 50);
}

int random_color() {
  //return random(256);
  return random(178) + 66;
}

void loop() {

  if (mode != 1) {
    while (Serial.available()) {
      delay(3);  //delay to allow buffer to fill
      if (Serial.available() > 0) {
        char c = Serial.read();  //gets one byte from serial buffer
        readString += c; //makes the string readString
      }

    }

    if (readString.length() == 6) {
      Serial.print("RECEIVED ");
      Serial.println(readString);
      long number = (long) strtol( &readString[0], NULL, 16);

      col_x[0] = number >> 16;
      col_x[1] = number >> 8 & 0xFF;
      col_x[2] = number & 0xFF;

      fadeToColor(leds, col, col_x, 10);
      readString = "";
    }

  } else {

    int z = random(3);

    col_x[0] = z == 0 ? 0 : random(256);
    col_x[1] = z == 1 ? 0 : random(256);
    col_x[2] = z == 2 ? 0 : random(256);
    

    //int _speed = random(20, 100);
    int _speed = random(75, 100);

    fadeToColor(leds, col, col_x, _speed);
  }

  col[0] = col_x[0];
  col[1] = col_x[1];
  col[2] = col_x[2];

  EEPROM.write(0,  col[0]);
  EEPROM.write(10,  col[1]);
  EEPROM.write(20,  col[2]);
}

String toHex(byte col[]) {
  return "#" + String(col[0], HEX) + String(col[1], HEX) + String(col[2], HEX) ;
}
/* Sets the color of the LED to any RGB Value   
   led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))
  color - (byte array of three values defing an RGB color to display 
  (color[0] = new Red value, color[1] = new Green value, color[2] = new Red value
  */
void setColor(int* led, byte * color) {
  for (int i = 0; i < 3; i++) {           //iterate through each of the three pins (red green blue)
    analogWrite(led[i],  color[i]);//255-

    //set the analog output value of each pin to the input value (ie led[0] (red pin) to 255- color[0] (red input color)
    //we use 255 - the value because our RGB LED is common anode, this means a color is full on when we output analogWrite(pin, 0)
    //and off when we output analogWrite(pin, 255).
  }
}
/* A version of setColor that takes a predefined color (neccesary to allow const int pre-defined colors */
void setColor(int* led, const byte * color) {
  byte tempByte[] = {color[0], color[1], color[2]};
  setColor(led, tempByte);
}
/* Fades the LED from a start color to an end color at fadeSpeed
  led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))
  startCcolor - (byte array of three values defing the start RGB color (startColor[0] = start Red value, startColor[1] = start Green value, startColor[2] = start Red value
  endCcolor - (byte array of three values defing the finished RGB color (endColor[0] = end Red value, endColor[1] = end Green value, endColor[2] = end Red value
  fadeSpeed - this is the delay in milliseconds between steps, defines the speed of the fade*/
void fadeToColor(int* led, byte * startColor, byte * endColor, int fadeSpeed) {
  int changeRed = endColor[0] - startColor[0];
  //the difference in the two colors for the red channel
  int changeGreen = endColor[1] - startColor[1];
  //the difference in the two colors for the green channel
  int changeBlue = endColor[2] - startColor[2];
  //the difference in the two colors for the blue channel
  int steps = max(abs(changeRed), max(abs(changeGreen), abs(changeBlue)));
  //make the number of change steps the maximum channel change
  for (int i = 0 ; i < steps; i++) {
    //iterate for the channel with the maximum change
    byte newRed = startColor[0] + (i * changeRed / steps);
    //the newRed intensity dependant on the start intensity and the change determined above
    byte newGreen = startColor[1] + (i * changeGreen / steps);
    //the newGreen intensity
    byte newBlue = startColor[2] + (i * changeBlue / steps);
    //the newBlue intensity
    byte newColor[] = {newRed, newGreen, newBlue};
    //Define an RGB color array for the new color
    setColor(led, newColor);
    //Set the LED to the calculated value
    delay(fadeSpeed);
    //Delay fadeSpeed milliseconds before going on to the next color
  }
  setColor(led, endColor);
  //The LED should be at the endColor but set to endColor to avoid rounding errors
}
/* A version of fadeToColor that takes predefined colors (neccesary to allow const int pre-defined colors */
void fadeToColor(int* led, const byte * startColor, const byte * endColor, int fadeSpeed) {
  byte tempByte1[] = {startColor[0], startColor[1], startColor[2]};
  byte tempByte2[] = {endColor[0], endColor[1], endColor[2]};   fadeToColor(led, tempByte1, tempByte2, fadeSpeed);
}


