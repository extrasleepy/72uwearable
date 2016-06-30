#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define ANALOG_INPUT A4  //Analog 4 is connected to sound sensor/microphone

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.

  Serial.begin(9600);     // Setup serial port.
}

void loop() {
  
  // Get the sensor sensor value and print it to Serial Monitor
  uint16_t value = analogRead(ANALOG_INPUT);
  Serial.print("raw soundlevel= ");
  Serial.println(value, DEC);

  delay(500); // half second delay between readings

}
