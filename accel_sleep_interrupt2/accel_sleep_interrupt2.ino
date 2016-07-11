
#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Narcoleptic.h>

volatile boolean sleepMove = false;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  Serial.begin(9600);     // Setup serial port.
  CircuitPlayground.lis.setClick(1, 10, 10, 1, 1);
  attachInterrupt(4, movement, CHANGE);
}

void loop() {
  sleepMove = false;
  delay(1000);
  goToSleep();
  
}

void movement() {
  sleepMove = true;
  Serial.println("awake");
}

void goToSleep() {
  Serial.println("sleeping");
  Narcoleptic.delay(1000);
 
}


