#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define ANALOG_INPUT A4  //Analog 4 is connected to sound sensor/microphone
const int sampleWindow = 1000; // Sample window for mic.
unsigned int sample;


void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.

  Serial.begin(9600);     // Setup serial port.
}

void loop() {

  // Get the sensor sensor value and print it to Serial Monitor
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(A4);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude


  Serial.println(peakToPeak);
}
