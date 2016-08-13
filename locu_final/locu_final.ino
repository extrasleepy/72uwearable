//Code for 72u LOCU project. Summer 2016.

#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Adafruit_SleepyDog.h>  //library that allows low power sleeping

#define TEMP A0   //Analog 0 is connected to temperature sensor
#define SOUND A4  //Analog 4 is connected to sound sensor/microphone
#define LIGHT A5  //Analog 5 is connected to light sensor

float tMin = 69;   //min max variables for sensors temp = 70-74 degrees, sound=300-550 based on db, light=20-180 based on 150 lux
float tMax = 75;
float sMin = 40;
float sMax = 100;
float lMin = 40;
float lMax = 260;

float tempCalib = 6; //variable to calibrate temperature

uint8_t xPrev = 0;  //variables to test movement for sleep mode
uint8_t yPrev = 0;
uint8_t zPrev = 0;
uint8_t xMove = 0;
uint8_t yMove = 0;
uint8_t zMove = 0;

int moveFlex = 2; //a little flexibility for minor vibrations
int moveTimer = 0;  //timer keeps track how long since last movement
long verySleepy = 1500;// (1500 = 90ish min)
long quietTimeInterval = 750; //(750 = 40ish min)
long quietTimer = 0; //timer keeps track how long since last tone

int pixels[] = {0, 2, 4, 5, 7, 9};   //array of used light pins
int perfectTones[] =       {300, 380, 440, 620};   //array for perfect environment sound
int perfectTonesLength[] = {125, 125, 200, 300};  //array for adding sound durations

bool resetSpin = true;  //keeps track of how often light spin happens
bool quietTime = false; //keeps tone from playing too often

long fadeInterval = 30000; // interval between light fades (60ish seconds)
long sinceLastFade = 29500; //fades one time right away when powered up

uint16_t tempValue = 0;   //used to average sensors
uint16_t lightValue = 0;

const int sampleWindow = 1000; // Sample window for mic.
unsigned int sample;

int fadeRate = 1500;
int between = 15;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(170); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.
}

void loop() {

  //check for movement
  xPrev = xMove; yPrev = yMove; zPrev = zMove;
  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  //if no movement increment sleep timer
  if (xMove >= xPrev - moveFlex && xMove <= xPrev + moveFlex && yMove >= yPrev - moveFlex && yMove <= yPrev + moveFlex && zMove >= zPrev - moveFlex && zMove <= zPrev + moveFlex) {
    moveTimer++;
    Serial.println(moveTimer);
    if (moveTimer > verySleepy) {  //if enough time has passed, go to short powersave sleep
      sleepyTime();
    }
  } else {
    Serial.println("moving");
    moveTimer = 0;
  }

  if (moveTimer >= (verySleepy + 500)) { //safegaurd
    moveTimer = 0;
  }

  if (moveTimer < verySleepy) {
    // Get the sensor sensor values
    uint16_t tempSample = 0;
    uint16_t soundSample = 0;
    uint16_t lightSample = 0;  //used to average sensors

    for (int i = 0; i < 20; i++) {  //take 20 samples over 2 seconds
      tempSample += CircuitPlayground.temperatureF() - tempCalib;
      lightSample += analogRead(LIGHT);
      delay(100);
    }
    tempValue = tempSample / 20;   //average the 10 samples
    lightValue = lightSample / 20;

    unsigned long startMillis = millis(); // Start of sample window
    unsigned int soundValue = 0;   // peak-to-peak level

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
    soundValue = signalMax - signalMin;  // max - min = peak-peak amplitude

    //print sensor values to serial monitor
    Serial.print("raw temp= ");
    Serial.println(tempValue, DEC);
    Serial.print("raw sound= ");
    Serial.println(soundValue, DEC);
    Serial.print("raw light= ");
    Serial.println(lightValue, DEC);

    if ((millis() - sinceLastFade) > (fadeInterval))  //use millis to determine when to fade lights
    {
      sinceLastFade = millis();
      lightUp(tempValue, soundValue, lightValue);   //function to lights fades
    }
  }

  if (quietTime == true) {
    quietTimer++;
    Serial.print("qt=");
    Serial.println(quietTimer);
  } else {
    Serial.print("qt=");
    Serial.println(quietTimer);
    Serial.println("tone will play");
  }
  if (quietTimer > quietTimeInterval) {
    quietTime = false;
    quietTimer = 0;
    Serial.println("tone reset");
  }

}

//function to control lights and sound based on sensor readings
uint16_t lightUp(uint16_t tempValue, uint16_t soundValue, uint16_t lightValue) {

  //play tone if environment is perfect
  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax && resetSpin == true) {

    if (quietTime == false) {
      for (int i = 0; i < sizeof(perfectTones) / sizeof(int); i++) {
        CircuitPlayground.playTone(perfectTones[i], perfectTonesLength[i]);
        delay(perfectTonesLength[i]);
      }
      quietTime = true;
    }

    //light spin twice
    for (int spin = 0; spin < 3; spin++) {   //spin happens twice
      for (int i = 0; i < 10; i++) {    //fade each of the 6 lights
        for (int fd = 0; fd <= 255; fd += 3) {
          CircuitPlayground.strip.setPixelColor(i, fd, fd, fd);   //white
          CircuitPlayground.strip.setPixelColor(9 - i, fd, fd, fd); //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(1);
        }
        for (int fd = 255; fd >= 0; fd -= 3) {
          CircuitPlayground.strip.setPixelColor(i, fd, fd, fd);   //white
          CircuitPlayground.strip.setPixelColor(9 - i, fd, fd, fd); //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(1);
        }
      }
    }
    resetSpin = false;
  }

  //white fade in
  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
    for (int fd = 0; fd < 255; fd++) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
      }
      delay(10);
    }
    //white fade out
    for (int fd = 255; fd > 0; fd--) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();                                 // update pixels!

      }
      delay(10);
    }
  } else {
    resetSpin = true;
    //-----temp and light--------
    if (lightValue > lMin && lightValue < lMax && tempValue > sMin && tempValue < sMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);     //yellow (light)
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0 ); //yellow (light)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0) ; //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0);  //yellow (light)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);  //yellow (light)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0 ); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, fd, fd,  0);  //yellow (light)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
    }
    //-----light and sound--------
    else if (lightValue > lMin && lightValue < lMax && soundValue > sMin && soundValue < sMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);     //yellow (light)
        CircuitPlayground.strip.setPixelColor(0, fd, 0 , fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0 ); //yellow (light)
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd );
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0) ; //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);  //yellow (light)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0 ); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, fd, fd,  0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd ); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
    }
    //-----temp and sound--------
    else if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.setPixelColor(0, fd, 0 , fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd );
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd ); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
    }

    //-------temp only---------
    else if (tempValue > tMin && tempValue < tMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd ); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 127 + fd, 127 + fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(7, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.setPixelColor(5, 0, 255 - fd, 255 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(5, 0, 127 - fd, 127 - fd); //cyan (temp)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
    }
    //-------sound only---------
    else if (soundValue > sMin && soundValue < sMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0 , fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd ); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);     //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(9, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd ); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(9, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 127 + fd, 0, 127 + fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(9, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.setPixelColor(0, 255 - fd, 0, 255 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(0, 127 - fd, 0, 127 - fd); //magenta (sound)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
    }
    //-------light only---------
    else if (lightValue > lMin && lightValue < lMax) {
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);     //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0 ); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0) ; //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);  //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      delay(between);
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 + fd, 127 + fd, 0 ); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, fd, fd,  0);   //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 127 + fd, 127 + fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(4, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.setPixelColor(2, 255 - fd, 255 - fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
      for (int fd = 0; fd < 127; fd++) {
        CircuitPlayground.strip.setPixelColor(2, 127 - fd, 127 - fd, 0); //yellow (light)
        CircuitPlayground.strip.show();  // update pixels!
        delayMicroseconds(fadeRate);
      }
    }
  }
}

//function to save power when LOCU is at rest for specified time
void sleepyTime() {
  CircuitPlayground.clearPixels();

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  if (xMove >= xPrev - moveFlex && xMove <= xPrev + moveFlex && yMove >= yPrev - moveFlex && yMove <= yPrev + moveFlex && zMove >= zPrev - moveFlex && zMove <= zPrev + moveFlex) {
    Watchdog.sleep(1000);    //save power for 1 second
    quietTime = false;
  } else {
    moveTimer = 0;
  }


  xPrev = xMove;
  yPrev = yMove;
  zPrev = zMove;

}



