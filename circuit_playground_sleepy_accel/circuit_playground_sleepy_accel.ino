#include <Adafruit_CircuitPlayground.h>   //required library to use module
//#include <Narcoleptic.h>

uint8_t xPrevious = 0;
uint8_t yPrevious = 0;
uint8_t zPrevious = 0;
uint8_t xMove = 0;
uint8_t yMove = 0;
uint8_t zMove = 0;

int movetimer = 0;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(100); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.

}

void loop() {

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();
  //Serial.println(xMove);
  //Serial.println(yMove);
  //Serial.println(zMove);

  if (xMove >= xPrevious - 1 && xMove <= xPrevious + 1 && yMove >= yPrevious - 1 && yMove <= yPrevious + 1 && zMove >= zPrevious - 1 && zMove <= zPrevious + 1) {
    movetimer++;
    if (movetimer > 25) {
      sleepyTime();
    }

  }
  else {
    Serial.println("moving");
    movetimer = 0;
    CircuitPlayground.strip.setPixelColor(1, 255, 255, 255);  //white
    CircuitPlayground.strip.show();  // update pixels!
  }

  xPrevious = xMove;
  yPrevious = yMove;
  zPrevious = zMove;

  delay(500);

}

void sleepyTime() {

  Serial.println("sleeping");
  CircuitPlayground.strip.setPixelColor(1, 0, 0, 0); //white
  CircuitPlayground.strip.show();  // update pixels!

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  if (xMove >= xPrevious - 1 && xMove <= xPrevious + 1 && yMove >= yPrevious - 1 && yMove <= yPrevious + 1 && zMove >= zPrevious - 1 && zMove <= zPrevious + 1) {
    delay(500);
  }
  else {
    movetimer = 0;
  }
  xPrevious = xMove;
  yPrevious = yMove;
  zPrevious = zMove;
}

