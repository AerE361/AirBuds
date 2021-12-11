#include <Adafruit_NeoPixel.h>


#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>
#include <Arduino.h>
#include <math.h>

#define PIN 6
#define NUMPIXELS 30

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




//Variables for timing and direction
int location = 0;
bool dir = true;
bool colorChanged = false;
int period = 500;
int fireBurnSpeed = 1; // Number of colors fire will burn in 10 seconds
int currentColor = 1;
unsigned long time_now = 0;
unsigned long lastPress = 0;
unsigned long time_now_burn = 0;
int ColorAPos[10] = {BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};
bool UnlockedColors[10] = {true,true,true,false,false,false,false,false,false,false};

//The function that checks each time the board is changed to see if the player has unlocked new colors
void checkforcolorunlock() {
  for(int i = 0; i<10 ;i++) {
    if (!UnlockedColors[3]) {
      if( ColorAPos[(i-1)%10] == YELLOW && ColorAPos[i] == YELLOW && ColorAPos[(i+1)%10] == YELLOW) {
        UnlockedColors[3] = true;
        ColorAPos[(i-1)%10] = BLACK;
        ColorAPos[i] = GREEN;
        ColorAPos[(i+1)%10] = BLACK;
      }
    }
    if (!UnlockedColors[4]) {
      if ((ColorAPos[(i-1)%10] == GREEN && ColorAPos[i] == YELLOW) || (ColorAPos[(i-1)%10] == YELLOW && ColorAPos[i] == GREEN)) {
        UnlockedColors[4] = true;
        ColorAPos[(i-1)%10] = BLUE;
        ColorAPos[i] = BLUE;
      }
    }
    if (!UnlockedColors[5]) {
      if( ColorAPos[(i-1)%10] == GREEN && ColorAPos[i] == GREEN && ColorAPos[(i+1)%10] == GREEN) {
        UnlockedColors[5] = true;
        ColorAPos[i] = RED;
      }
    }
    if (!UnlockedColors[6]) {
      if( ColorAPos[(i-1)%10] == RED && ColorAPos[i] == BLUE && ColorAPos[(i+1)%10] == RED) {
        UnlockedColors[6] = true;
        ColorAPos[i] = PURPLE;
      }
    }
    if (!UnlockedColors[7]) {
      //Something a little more complex
    }
    if (!UnlockedColors[8]) {
      if((ColorAPos[(i-1)%10] == BLUE && ColorAPos[i] == ORANGE) || (ColorAPos[(i-1)%10] == BLUE && ColorAPos[i] == ORANGE)) {
        ColorAPos[(i-1)%10] = BLUE;
        ColorAPos[i] = BLUE;
      }
    }
    if (!UnlockedColors[9]) {
      //Something a little more complex
    }
  }
}

//Our Burning function
void burn() {
  if (UnlockedColors[5]) {
    for (int i = 0; i<10; i++) {
      if((ColorAPos[i] == RED)) {
        if (ColorAPos[(i-1)%10] == GREEN || ColorAPos[(i+1)%10] == CYAN) {
          ColorAPos[(i-1) == RED];
          break;
        }
        if (ColorAPos[(i+1)%10] == GREEN || ColorAPos[(i+1)%10] == CYAN) {
          ColorAPos[(i-1) == RED];
          break;
        }
      }
    }
  }
}

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);
  strip.begin();
  strip.show();
  strip.setBrightness(50);
}

void loop() {
  //Color Changer / Timer
  if(millis() >= time_now + period){
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
    
    if (dir) {
      setColor((location-1)%10,BLACK);
    }
    else {
      setColor((location+1)%10,BLACK);
    }
    
    setColor(location,currentColor);
    time_now += period;
    colorChanged = false;
  }
  
  //Button Stuff
  if(millis() >= lastPress+300) {
    if(CircuitPlayground.rightButton()) {
      lastPress = millis();
      dir = !dir;
    }

    if(CircuitPlayground.leftButton()) {
      lastPress = millis();
      setColor(location,BLACK);
      colorChanged = true;
      currentColor++;
      if(currentColor == 10) {
        currentColor = 1;
      }
      setColor(location,currentColor);
    }
  }
  
  //Burn checker
  if(millis() >= time_now_burn + (10/fireBurnSpeed)*1000) {
    burn();
  }
}
