#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define ANALOG_INPUT A0  //Analog 0 is connected to temperature sensor

float tempCalib = 7.6;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.

  Serial.begin(9600);     // Setup serial port.

}

void loop() {
  
  // Get the sensor sensor value and print it to Serial Monitor
  uint16_t value = analogRead(ANALOG_INPUT);
  Serial.print("raw templevel= ");
  Serial.println(value, DEC);
  Serial.println(CircuitPlayground.temperatureF()-tempCalib);

  delay(500); // half second delay between readings

}
