#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include <math.h>

int i = 0;
bool dir = true;
bool canChange = true;
int period = 500;
unsigned long time_now = 0;
unsigned long listenNow = 0;
unsigned long stepHere = 0;


void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);
}

void loop() {

  if(millis() >= listenNow){
    if (CircuitPlayground.rightButton() && canChange) {
      canChange = false;
      listenNow = listenNow + period + 100;
      Serial.print("You pressed the button");
      dir = !dir;
    }
  }
  
  if(millis() >= time_now + period){
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(i,10);
    time_now += period;
    stepHere = time_now + 350;
  }

  if(millis() >= stepHere) {
    stepHere += 10000;
    if (dir) {
      i++;
    }
    else {
      i--;
    }
    canChange = true;
  }

  i = i%10;
  if (i<0) {
    i = 9;
  }
}