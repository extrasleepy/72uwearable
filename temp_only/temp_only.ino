#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define ANALOG_INPUT A0  //Analog 5 is connected to temperature sensor

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.

  Serial.begin(9600);     // Setup serial port.
  Serial.println("Temperaure Sensor Raw Value");
}

void loop() {
  
  // Get the sensor sensor value and print it to Serial Monitor
  uint16_t value = analogRead(ANALOG_INPUT);
  Serial.println(value, DEC);

  delay(500); // half second delay between readings

}
