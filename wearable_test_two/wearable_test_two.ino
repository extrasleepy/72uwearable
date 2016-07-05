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

int pixels[] = {0, 2, 4, 5, 7, 9};   //array of used pins

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(200); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.

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

  checkValues(tempValue, soundValue, lightValue);

  CircuitPlayground.strip.show();  // update pixels!
  delay(500); // half second delay between readings

  for (int i = 0; i < 10; i++) {   //turn off pixels (only visible if above parameters not met)
    CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
  }

}

uint16_t checkValues(uint16_t tempValue, uint16_t soundValue, uint16_t lightValue) {
  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
    for (int spin = 0; spin < 2; spin++) {   //spin happens twice
      for (int i = 0; i < 7; i++) {    //fade each of the 6 lights
        for (int fd = 0; fd < 255; fd += 5) {
          CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(500);
        }
        for (int fd = 255; fd > 0; fd -= 5) {
          CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(500);
        }
      }
    }
    for (int i = 0; i < 7; i++) {
      for (int fd = 0; fd > 255; fd += 5) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(500);
      }
    }

  } else {
    if (tempValue > tMin && tempValue < tMax) {     //see if temp is within range
      for (int fd = 0; fd < 255; fd++) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(2000);
      }
      for (int fd = 255; fd > 0; fd--) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(2000);
      }
    }
  }

  if (soundValue > sMin && soundValue < sMax) {    //see if sound is within range
    for (int fd = 0; fd < 255; fd++) {
      CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow
      CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow
      CircuitPlayground.strip.show();  // update pixels!
      delayMicroseconds(2000);
    }
    for (int fd = 255; fd > 0; fd--) {
      CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow
      CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow
      CircuitPlayground.strip.show();  // update pixels!
      delayMicroseconds(2000);
    }
  }

  if (lightValue > lMin && lightValue < lMax) {   //see if light is within range

    for (int fd = 0; fd < 255; fd++) {
      CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan
      CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan
      CircuitPlayground.strip.show();  // update pixels!
      delayMicroseconds(2000);
    }
    for (int fd = 255; fd > 0; fd--) {
      CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);    //yellow
      CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);    //yellow
      CircuitPlayground.strip.show();  // update pixels!
      delayMicroseconds(2000);
    }

  }
}

