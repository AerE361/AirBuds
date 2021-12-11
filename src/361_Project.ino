#include <Adafruit_NeoPixel.h>


#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>
#include <Arduino.h>
#include <math.h>

//If we knew how to get the SPI flash working we would use these.
//From what we found online there were basically nothing that would let us use the flash.  SPI flash library didn't work due to variant.h not having a QSPI/SPI flash defined in it.
//Very unsure how to fix that, but we're quite interested.
//#include <ArduinoJson.h>
//#include <SPI.h>
//#include <SdFat.h>
//#include <Adafruit_SPIFlash.h>

#define PIN 6
#define NUMPIXELS 30

//Defining all Colors so that code is easier to read.
#define BLACK 0
#define WHITE 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define RED 5
#define PURPLE 6
#define ORANGE 7
#define CYAN 8
#define PINK 9

//Color Setting Functions for easy reading.
void setColor(int pos,int color) {
  switch(color)  {
    case BLACK:
      CircuitPlayground.setPixelColor(pos,0,0,0);
      break;
    case WHITE:
      CircuitPlayground.setPixelColor(pos,255,255,255);
      break;
    case YELLOW:
      CircuitPlayground.setPixelColor(pos,255,255,0);
      break;
    case GREEN:
      CircuitPlayground.setPixelColor(pos,0,255,0);
      break;
    case BLUE:
      CircuitPlayground.setPixelColor(pos,0,0,255);
      break;
    case RED:
      CircuitPlayground.setPixelColor(pos,255,0,0);
      break;
    case PURPLE:
      CircuitPlayground.setPixelColor(pos,255,0,255);
      break;
    case ORANGE:
      CircuitPlayground.setPixelColor(pos,255,128,0);
      break;
    case CYAN:
      CircuitPlayground.setPixelColor(pos,0,255,255);
      break;
    case PINK:
      CircuitPlayground.setPixelColor(pos,244,0,180);
      break;
  }
}

//Variables for color burning logic
int greenBurned = 0;
int cyanBurned = 0;

//Variables for timing and direction
int location = 0;
bool dir = true;
int period = 500;

// Number of colors fire will burn in 2 seconds
int spreadRate = 1;
int currentColor = 1;
bool colorChanged = false;

//More timing variables
unsigned long time_now = 0;
unsigned long lastPress = 0;
unsigned long timerForCheck = 0;
unsigned long fireTimer = 0;
unsigned long lastTeleport = 0;
unsigned long lastPinkDirChange = 0;
unsigned long timeSpread = 0;


//Arrays for tracking what is happening
int ColorAPos[10] = {BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};
int ColorPlaced[10] = {0,0,0,0,0,0,0,0,0,0};
bool UnlockedColors[10] = {true,true,true,false,false,false,false,false,false,false};
//Update the Circle Colors after an event
void updateCircle() {
  for(int i = 0; i<10; i++) {
    if (i != location) {
      setColor(i, ColorAPos[i]);
    }
  }
}


//The function that checks each time the board is changed to see if the player has unlocked new colors
void checkforcolorunlock() {
  for(int i = 0; i<10 ;i++) {
    if (!UnlockedColors[3]) {
      if( ColorAPos[(i-1 + 10)%10] == YELLOW && ColorAPos[i] == YELLOW && ColorAPos[(i+1)%10] == YELLOW) {
        UnlockedColors[3] = true;
        ColorAPos[(i-1 + 10)%10] = BLACK;
        ColorAPos[i] = GREEN;
        ColorAPos[(i+1)%10] = BLACK;
        Serial.println("GREEN UNLOCKED");
      }
    }
    if (!UnlockedColors[4]) {
      if ((ColorAPos[(i-1 + 10)%10] == GREEN && ColorAPos[i] == YELLOW) || (ColorAPos[(i-1 + 10)%10] == YELLOW && ColorAPos[i] == GREEN)) {
        UnlockedColors[4] = true;
        ColorAPos[(i-1 + 10)%10] = BLUE;
        ColorAPos[i] = BLUE;
        Serial.println("BLUE UNLOCKED");
      }
    }
    if (!UnlockedColors[5]) {
      if( ColorAPos[(i-1 + 10)%10] == GREEN && ColorAPos[i] == GREEN && ColorAPos[(i+1)%10] == GREEN) {
        UnlockedColors[5] = true;
        ColorAPos[i] = RED;
        Serial.println("RED UNLOCKED");
      }
    }
    if (!UnlockedColors[6]) {
      if( ColorAPos[(i-1 + 10)%10] == RED && ColorAPos[i] == BLUE && ColorAPos[(i+1)%10] == RED) {
        UnlockedColors[6] = true;
        ColorAPos[i] = PURPLE;
        Serial.println("PURPLE UNLOCKED");
      }
    }
    if (!UnlockedColors[8]) {
      if((ColorAPos[(i-1 + 10)%10] == BLUE && ColorAPos[i] == ORANGE) || (ColorAPos[(i+1)%10] == BLUE && ColorAPos[i] == ORANGE)) {
        UnlockedColors[8] = true;
        ColorAPos[(i-1 + 10)%10] = CYAN;
        ColorAPos[i] = CYAN;
        Serial.println("CYAN UNLOCKED");
      }
    }
  }
  updateCircle();
  updateStrip();
}

//Applys and effects to the board
void eventChecker() {
  
  //Teleporter
  if (ColorAPos[location] == PURPLE && millis() >= lastTeleport + 550) {
    if (dir) {
      for (int i = location+1; i<location+9; i++) {
        if (ColorAPos[i%10] == PURPLE) {
          location = i%10;
          setColor(location, WHITE);
          lastTeleport = millis();
          break;
        }
      }
    }
    else {
      for (int i = location-1; i>location-9; i--) {
        if (ColorAPos[(i+10)%10] == PURPLE) {
          location = (i+10)%10;
          setColor(location, WHITE);
          lastTeleport = millis();
          break;
        }
      }
    }
  }
  //Pink Direction Changer
  if (ColorAPos[location] == PINK && millis() >= lastPinkDirChange + 550) {
    dir = !dir;
    lastPinkDirChange = millis();
  }
  
  //Spreaders
  if (millis() >= timeSpread + 2/spreadRate*1000) {
    bool blueCheck = false, redCheck = false, cyanCheck = false;
    timeSpread += 2/spreadRate*1000;
    for (int i = 0; i<10; i++) {
    //Fire dowser
    if (ColorAPos[i] == BLUE && !blueCheck) {
      if (ColorAPos[(i-1+10)%10] == RED) {
        ColorAPos[(i-1+10)%10] = BLUE;
        blueCheck = true;
      }
      else if (ColorAPos[(i+1)%10] == RED) {
        ColorAPos[(i+1)%10] = BLUE;
        blueCheck = true;
      }
    }
    
    //Fire Spreader
    if (ColorAPos[i] == RED && !redCheck) {
      if (ColorAPos[(i+1)%10] == GREEN) {
        ColorAPos[(i+1)%10] = RED;
        greenBurned++;
        if (greenBurned > 3 && !UnlockedColors[7]) {
          ColorAPos[(i+1)%10] = ORANGE;
          UnlockedColors[7] = true;
        }
        redCheck = true;
      }
      else if (ColorAPos[(i-1+10)%10] == GREEN) {
        ColorAPos[(i-1+10)%10] = RED;
        greenBurned++;
        if (greenBurned > 3 && !UnlockedColors[7]) {
          ColorAPos[(i-1+10)%10] = ORANGE;
          UnlockedColors[7] = true;
        }
        redCheck = true;
      }
      else if (ColorAPos[(i-1+10)%10] == CYAN) {
        ColorAPos[(i-1+10)%10] = RED;
        cyanBurned++;
        if (cyanBurned > 3 && !UnlockedColors[9]) {
          ColorAPos[(i-1+10)%10] = PINK;
          UnlockedColors[9] = true;
        }
        redCheck = true;
      }
      else if (ColorAPos[(i+1)%10] == CYAN) {
        ColorAPos[(i+1)%10] = RED;
        cyanBurned++;
        if (cyanBurned > 3 && !UnlockedColors[9]) {
          ColorAPos[(i+1)%10] = PINK;
          UnlockedColors[9] = true;
        }
        redCheck = true;
      }
    }
    
    //Cyan Spreader
    if (ColorAPos[i] == CYAN && !cyanCheck) {
      if (ColorAPos[(i+1)%10] == BLACK || ColorAPos[(i+1)%10] == BLACK) {
        ColorAPos[(i+1)%10] = CYAN;
        cyanCheck = true;
      }
      else if (ColorAPos[(i-1+10)%10] == BLACK || ColorAPos[(i-1+10)%10] == BLACK) {
        ColorAPos[(i-1+10)%10] = CYAN;
        cyanCheck = true;
      }
    }
    }
    updateCircle();
    updateStrip();
  }
}


Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void updateStrip(void) {
  for (int i = 0; i<10; i++) {
    if (UnlockedColors[i]) {
      stripSetColor(i,i);
    }
  }
}

void stripSetColor(int pos,int color) {
  switch(color)  {
    case BLACK:
      strip.setPixelColor(pos,0,0,0);
      break;
    case WHITE:
      strip.setPixelColor(pos,255,255,255);
      break;
    case YELLOW:
      strip.setPixelColor(pos,255,255,0);
      break;
    case GREEN:
      strip.setPixelColor(pos,0,255,0);
      break;
    case BLUE:
      strip.setPixelColor(pos,0,0,255);
      break;
    case RED:
      strip.setPixelColor(pos,255,0,0);
      break;
    case PURPLE:
      strip.setPixelColor(pos,255,0,255);
      break;
    case ORANGE:
      strip.setPixelColor(pos,255,128,0);
      break;
    case CYAN:
      strip.setPixelColor(pos,0,255,255);
      break;
    case PINK:
      strip.setPixelColor(pos,244,0,180);
      break;
  }
  strip.show();
}

//File myFile;

void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(10);
  Serial.print("Initializing Flash");
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  
}
bool lastState = CircuitPlayground.slideSwitch();

void loop() {
  //Color Changer / Timer
  if(millis() >= time_now + period){
    if (colorChanged != false) {
      setColor(location,currentColor);
      ColorAPos[location] = currentColor;
    }
    if (dir) {
      location++;
    }
    else {
      location--;
    }
    location = location%10;
    if (location<0) {
      location = 9;
    }
    
    if (ColorAPos[location] == GREEN) {
      time_now += period*2;
    }
    else if (ColorAPos[location] == ORANGE) {
      time_now += period*.5;
    }
    else {
      time_now += period;
    }
    
    setColor(location,WHITE);
    colorChanged = false;
    currentColor = 1;
    for (int i = 0; i<10; i++) {
      Serial.print(ColorAPos[i]);
    }
    Serial.println(" ");
    
  }
  
  if(millis() >= lastPress+300) {
    if(CircuitPlayground.rightButton()) {
      lastPress = millis();
      dir = !dir;
    }

    if(CircuitPlayground.leftButton()) {
      lastPress = millis();
      time_now = millis();
      currentColor++;
      while(UnlockedColors[currentColor] != true) {
        currentColor++;
        currentColor = currentColor%10;
      }
      setColor(location,currentColor);
      colorChanged = true;
    }

    if(CircuitPlayground.slideSwitch() == !lastState) {
      for (int i = 0; i <10; i++) {
        ColorAPos[i] = 0;
      }
      CircuitPlayground.clearPixels();
      lastState = !lastState;
    }
  }

  
  checkforcolorunlock();
  eventChecker();
}
