#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <avr/sleep.h>

volatile boolean sleepMove = false;
#define SENSITIVITY 135

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  Serial.begin(9600);     // Setup serial port.
  CircuitPlayground.lis.setClick(1, SENSITIVITY,20,20,255);
  pinMode(7, INPUT);
}

void loop() {
  Serial.println("awake");
  sleepMove=false;
  delay(1000);
  goToSleep();
}

void movement(){
  sleepMove=true;
}

void goToSleep() {
  Serial.println("sleeping");
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(4, movement, LOW);
  sleep_mode();
  sleep_disable();
  detachInterrupt(4);
}


