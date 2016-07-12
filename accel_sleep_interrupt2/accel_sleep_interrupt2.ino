
#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Narcoleptic.h>
#include <avr/sleep.h>

volatile boolean sleepMove = false;

#define SENSITIVITY 80

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  Serial.begin(9600);     // Setup serial port.
  // 1 = single click only interrupt output
  CircuitPlayground.lis.setClick(1, SENSITIVITY);
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
  Narcoleptic.delay(1000);
  goToSleep();
}


