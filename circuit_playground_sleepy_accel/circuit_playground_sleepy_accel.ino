#include <Adafruit_CircuitPlayground.h>   //required library to use module
//#include <Narcoleptic.h>

int xPrevious = 0;
int yPrevious = 0;
int zPrevious = 0;
int xMove = 0;
int yMove = 0;
int zMove = 0;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(100); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.

}

void loop() {
  CircuitPlayground.strip.setPixelColor(1, 255, 255, 255);  //white
  CircuitPlayground.strip.show();  // update pixels!

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();
  Serial.println(xMove);
  // Serial.println(yMove);
  //Serial.println(zMove);

  if (xMove == xPrevious) {
    Serial.println("not moving");
  }

  uint8_t xPrevious = xMove;
  uint8_t yPrevious = yMove;
  uint8_t zPrevious = zMove;

  Serial.println(xPrevious);

  delay(100);

}
