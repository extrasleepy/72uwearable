#include <Adafruit_CircuitPlayground.h>   //required library to use module


void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  attachInterrupt(4, itHappened, CHANGE);
  Serial.begin(9600);     // Setup serial port.
  CircuitPlayground.lis.setClick(1, 10, 10, 1, 1);
}

boolean clickHappened = false;

void loop() {
    if ( clickHappened ) {
        clickHappened = false;
        Serial.println( "got a click!" );
    }
}

void itHappened() {
    clickHappened = true;
}

