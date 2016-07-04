#include <Adafruit_CircuitPlayground.h>   //required library to use module

#define TEMP A0  //Analog 0 is connected to temperature sensor
#define SOUND A4  //Analog 4 is connected to sound sensor/microphone
#define LIGHT A5  //Analog 5 is connected to light sensor

boolean temp_bool = false, sound_bool = false, light_bool = false;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  Serial.begin(9600);             // Setup serial port.
  resetPixels();
}

void loop() {
  checkSensors();                   //function to check sensors and
  CircuitPlayground.strip.show();   // update pixels!
  delay(500);                       // half second delay between readings
}

void checkSensors() {
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

  if (lightValue > 900 && lightValue < 1100) {   //see if light is within range
    for (int fade = 0; fade < 255; fade++) {
      CircuitPlayground.strip.setPixelColor(0, 0, fade, fade);
      CircuitPlayground.strip.setPixelColor(1, 0, fade, fade);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
     for (int fade = 255; fade > 0; fade--) {
      CircuitPlayground.strip.setPixelColor(0, 0, fade, fade);
      CircuitPlayground.strip.setPixelColor(1, 0, fade, fade);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
  }

  if (soundValue > 320 && soundValue < 360) {    //see if sound is within range
    for (int fade = 0; fade < 255; fade++) {
      CircuitPlayground.strip.setPixelColor(3, fade, fade, 0);
      CircuitPlayground.strip.setPixelColor(4, fade, fade, 0);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
    for (int fade = 255; fade > 0; fade--) {
      CircuitPlayground.strip.setPixelColor(3, fade, fade, 0);
      CircuitPlayground.strip.setPixelColor(4, fade, fade, 0);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
  }

  if (tempValue > 500 && tempValue < 550) {     //see if temp is within range
    for (int fade = 0; fade < 255; fade++) {
      CircuitPlayground.strip.setPixelColor(6, fade, 0, fade);
      CircuitPlayground.strip.setPixelColor(7, fade, 0, fade);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
     for (int fade = 255; fade > 0; fade--) {
      CircuitPlayground.strip.setPixelColor(6, fade, 0, fade);
      CircuitPlayground.strip.setPixelColor(7, fade, 0, fade);
      CircuitPlayground.strip.show();   // update pixels!
      delay(3);
    }
  }

  if (light_bool == true && sound_bool == true && temp_bool == true) { //all sensor parameters met
    for (int i = 0; i < 10; i++) {   //all pixels white
      CircuitPlayground.strip.setPixelColor(i, 255, 255, 255);
    }
  }
}

void resetPixels() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
  }
}

