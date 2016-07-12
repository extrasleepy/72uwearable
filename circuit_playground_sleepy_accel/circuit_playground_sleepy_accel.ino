#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Narcoleptic.h>

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(100); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.

}

void loop() {
  CircuitPlayground.strip.setPixelColor(1, 255, 255, 255);  //white
  CircuitPlayground.strip.show();  // update pixels!

  uint8_t xMove = CircuitPlayground.motionX();
  uint8_t yMove = CircuitPlayground.motionY();
  uint8_t zMove = CircuitPlayground.motionZ();

  Serial.println(xMove);
  Serial.println(yMove);
  Serial.println(zMove);

  if (xMove == 0) {
    Serial.println("not moving");
  }

  delay(100);

}
