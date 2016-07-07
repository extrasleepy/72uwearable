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

bool tempRange = false;
bool soundRange = false;
bool lightRange = false;

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

  lightUp(tempValue, soundValue, lightValue);
  delay(1000); // 1 second delay between readings

}


uint16_t lightUp(uint16_t tempValue, uint16_t soundValue, uint16_t lightValue) {

  for (int fd = 0; fd < 255; fd++) {
    if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
      }
    }
    else {
      if (tempValue > tMin && tempValue < tMax) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta (temp)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta (temp)
      }
      if (soundValue > sMin && soundValue < sMax) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow (sound)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow (sound)
      }
      if (lightValue > lMin && lightValue < lMax) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (light)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (light)
      }
    }
    CircuitPlayground.strip.show();  // update pixels!
    delay(5);

  }
  for (int fd = 255; fd > 0; fd--) {
    if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
      }
    }
    else {
      if (tempValue > tMin && tempValue < tMax) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta (temp)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta (temp)
      }
      if (soundValue > sMin && soundValue < sMax) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow (sound)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow (sound)
      }
      if (lightValue > lMin && lightValue < lMax) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (light)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (light)
      }
    }
    CircuitPlayground.strip.show();  // update pixels!
    delay(5);
  }




}

