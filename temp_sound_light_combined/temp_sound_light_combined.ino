#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define TEMP A0  //Analog 0 is connected to temperature sensor
#define SOUND A4  //Analog 4 is connected to sound sensor/microphone
#define LIGHT A5  //Analog 5 is connected to light sensor

float tMin = 510;   //min max variables for sensors
float tMax = 580;
float sMin = 300;
float sMax = 400;
float lMin = 900;
float lMax = 1100;

int pixels[] = {0, 9, 2, 4, 5, 7};   //array of used pins

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.

  Serial.begin(9600);     // Setup serial port.

  CircuitPlayground.strip.setBrightness(200); //brightness is between 0 and 255

}

void loop() {

  // Get the sensor sensor values and print it to Serial Monitor
  uint16_t tempValue = analogRead(TEMP);
  uint16_t soundValue = analogRead(SOUND);
  uint16_t lightValue = analogRead(LIGHT);
  Serial.print("raw temp= ");
  Serial.println(tempValue, DEC);
  Serial.print("raw sound= ");
  Serial.println(soundValue, DEC);
  Serial.print("raw light= ");
  Serial.println(lightValue, DEC);

  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
    for (int i = 0; i < 7; i++) {
      CircuitPlayground.strip.setPixelColor(pixels[i], 255, 255, 255);   //white
    }
  } else {
    if (tempValue > tMin && tempValue < tMax) {     //see if temp is within range
      CircuitPlayground.strip.setPixelColor(0, 255, 0, 255);   //magenta
      CircuitPlayground.strip.setPixelColor(9, 255, 0, 255);   //magenta
    }

    if (soundValue > sMin && soundValue < sMax) {    //see if sound is within range
      CircuitPlayground.strip.setPixelColor(2, 255, 255, 0);    //yellow
      CircuitPlayground.strip.setPixelColor(4, 255, 255, 0);    //yellow
    }

    if (lightValue > lMin && lightValue < lMax) {   //see if light is within range
      CircuitPlayground.strip.setPixelColor(5, 0, 255, 255);     //cyan
      CircuitPlayground.strip.setPixelColor(7, 0, 255, 255);     //cyan
    }
  }

  CircuitPlayground.strip.show();  // update pixels!
  delay(500); // half second delay between readings

  for (int i = 0; i < 10; i++) {   //turn off pixels (only visible if above parameters not met)
    CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
  }

}
