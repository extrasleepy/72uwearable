//Code for 72u wearable electroincs project. Summer 2016

/* TODO::
   DONE: White lights spin 2 times when all parameters are correct BUT ONLY ONCE PER SESSION
   DONE: If you don't leave perfect environment, tone won't play again.
   DONE: How often do we see the lights? once every 60 sec
   DONE: Go sleep when no movement is detected over 15+ min, wake up on accelerometer movement (D3 D4 are interrupts)
   DONE: light should change to yellow, temp should be cyan, magenta sound
   DONE: Bug that causes lights to get stuck in crazy colors after some time. Might be related to SleepyDog?
   DONE: Sensor readings are single sample, need to be changed to an average of 5 to 10 samples
   DONE: What's the final color when all parameters are met?? currently white.
   DONE:Touch "remember an idea" sequence  - - reminder after 2 hours
   ->Make cool tones (JAVI working on this)
   ->Sensor numbers are calibrated but could use additional testing and adjustment


   Hardware Hacks:
   DONE -> Remove Battery connection from main circuit
   DONE -> Remove or break power LED
   DONE -> Battery goes from breakout board BATT to CirPlay VBATT
*/

#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Adafruit_SleepyDog.h>  //library that allows low power sleeping

#define TEMP A0  //Analog 0 is connected to temperature sensor
#define SOUND A4  //Analog 4 is connected to sound sensor/microphone
#define LIGHT A5  //Analog 5 is connected to light sensor
#define TONE_DURATION_MS 100  // Duration in milliseconds to play a tone when touched.

#define CAP_THRESHOLD    300  // Threshold for a capacitive touch (higher = less sensitive).
#define CAP_SAMPLES      20   // Number of samples to take for a capacitive touch read.
#define TONE_DURATION_MS 100  // Duration in milliseconds to play a tone when touched.

float tMin = 70;   //min max variables for sensors temp = 70-74 sound=300-550 light=20-120
float tMax = 74;   //all these numbers could use some testing
float sMin = 300;
float sMax = 550;
float lMin = 20;
float lMax = 120;

uint8_t xPrevious = 0;  //variables to test movement for sleep mode
uint8_t yPrevious = 0;
uint8_t zPrevious = 0;
uint8_t xMove = 0;
uint8_t yMove = 0;
uint8_t zMove = 0;

int moveFlex = 2; //a little flexibility for minor vibrations
int moveTimer = 0;
int verySleepy = 900; //1 represents about 1 second + total light fade times (900 = 20ish min)

int pixels[] = {0, 2, 4, 5, 7, 9};   //array of used light pins
int tones[] = {100, 500, 1000};   //array for adding sound
int tlength[] = {200, 250, 200};  //array for adding sound durations

int memTones[] = {300, 350, 500, 400, 550};   //array for adding sound
int memTonesLength[] = {200, 150, 150, 300, 300}; //array for adding sound durations
bool startMemTimer = false;
long memTimerInterval = 7200000;  //about 2 hours
long memTimerBegin = 0;

bool resetSpin = true;

long fadeInterval = 60000; //60ish seconds
long sinceLastFade = 59000; //fades one time right away when powered up

uint16_t tempValue = 0;
uint16_t soundValue = 0;
uint16_t lightValue = 0;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(180); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.
}

void loop() {

  ideaButton();
  rememberIdea();

  //check for movement
  xPrevious = xMove; yPrevious = yMove; zPrevious = zMove;
  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  //if no movement enter sleep
  if (xMove >= xPrevious - moveFlex && xMove <= xPrevious + moveFlex && yMove >= yPrevious - moveFlex && yMove <= yPrevious + moveFlex && zMove >= zPrevious - moveFlex && zMove <= zPrevious + moveFlex) {
    moveTimer++;
    Serial.println(moveTimer);
    if (moveTimer > verySleepy) {
      sleepyTime();
    }
  } else {
    Serial.println("moving");
    moveTimer = 0;
  }
  if (moveTimer < verySleepy) {

    // Get the sensor sensor values
    uint16_t tempSample = 0;
    uint16_t soundSample = 0;
    uint16_t lightSample = 0;

    for (int i = 0; i < 10; i++) {  //take 10 samples over 1 second

      tempSample += CircuitPlayground.temperatureF();
      soundSample += analogRead(SOUND);
      lightSample += analogRead(LIGHT);
      delay(100);
    }
    tempValue = tempSample / 10;   //average the 10 samples
    soundValue = soundSample / 10;
    lightValue = lightSample / 10;

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
}

uint16_t lightUp(uint16_t tempValue, uint16_t soundValue, uint16_t lightValue) {

  //play tone if environment is perfect   <----causing clicking??
  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax && resetSpin == true) {
    for (int i = 0; i <= sizeof(tones - 1); i++) {
      CircuitPlayground.playTone(tones[i], tlength[i]);
      delay(tlength[i]);
    }

    //spin twice
    for (int spin = 0; spin < 3; spin++) {   //spin happens twice
      for (int i = 0; i < 10; i++) {    //fade each of the 6 lights
        for (int fd = 0; fd <= 255; fd += 3) {
          CircuitPlayground.strip.setPixelColor(i, fd, fd, fd);   //white
          CircuitPlayground.strip.setPixelColor(9 - i, fd, fd, fd); //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(1);
          ideaButton(); rememberIdea();
        }
        for (int fd = 255; fd >= 0; fd -= 3) {
          CircuitPlayground.strip.setPixelColor(i, fd, fd, fd);   //white
          CircuitPlayground.strip.setPixelColor(9 - i, fd, fd, fd); //white
          CircuitPlayground.strip.show();  // update pixels!
          delayMicroseconds(1);
          ideaButton(); rememberIdea();
        }
      }
    }
    resetSpin = false;
  }

  //fade in loop
  for (int fd = 0; fd < 255; fd++) {
    if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
        ideaButton(); rememberIdea();
      }
    }
    else {
      resetSpin = true;

      if (tempValue > tMin && tempValue < tMax) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (temp)
        ideaButton(); rememberIdea();
      }
      if (soundValue > sMin && soundValue < sMax) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta (sound)
        ideaButton(); rememberIdea();
      }
      if (lightValue > lMin && lightValue < lMax) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow (light)
        ideaButton(); rememberIdea();
      }
    }
    CircuitPlayground.strip.show();  // update pixels!
    delay(10);
  }

  //fade out loop
  for (int fd = 255; fd > 0; fd--) {
    if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax) {
      for (int i = 0; i < 7; i++) {
        CircuitPlayground.strip.setPixelColor(pixels[i], fd, fd, fd);   //white
        CircuitPlayground.strip.show();  // update pixels!
        ideaButton(); rememberIdea();
      }
    }
    else {
      if (tempValue > tMin && tempValue < tMax) {
        CircuitPlayground.strip.setPixelColor(5, 0, fd, fd);     //cyan (temp)
        CircuitPlayground.strip.setPixelColor(7, 0, fd, fd);     //cyan (temp)
        ideaButton(); rememberIdea();
      }
      if (soundValue > sMin && soundValue < sMax) {
        CircuitPlayground.strip.setPixelColor(0, fd, 0, fd);   //magenta (sound)
        CircuitPlayground.strip.setPixelColor(9, fd, 0, fd);   //magenta (sound)
        ideaButton(); rememberIdea();
      }
      if (lightValue > lMin && lightValue < lMax) {
        CircuitPlayground.strip.setPixelColor(2, fd, fd, 0);    //yellow (light)
        CircuitPlayground.strip.setPixelColor(4, fd, fd, 0);    //yellow (light)
        ideaButton(); rememberIdea();
      }
    }
    CircuitPlayground.strip.show();  // update pixels!
    delay(10);
  }
}

void sleepyTime() {
  CircuitPlayground.clearPixels();

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  if (xMove >= xPrevious - moveFlex && xMove <= xPrevious + moveFlex && yMove >= yPrevious - moveFlex && yMove <= yPrevious + moveFlex && zMove >= zPrevious - moveFlex && zMove <= zPrevious + moveFlex) {
    Watchdog.sleep(1500);
  }
  else {
    moveTimer = 0;
  }
  xPrevious = xMove;
  yPrevious = yMove;
  zPrevious = zMove;

  ideaButton(); rememberIdea();
}

void ideaButton() {   //press capacitive touch pad 9 when you have a good idea 
  boolean memButton = CircuitPlayground.rightButton();
  if (CircuitPlayground.readCap(9, CAP_SAMPLES) >= CAP_THRESHOLD) {
    for (int i = 0; i <= sizeof(memTones); i++) {
      CircuitPlayground.playTone(memTones[i], memTonesLength[i]);
      delay(memTonesLength[i] + 5);
    }
    startMemTimer = true;
    memTimerBegin = millis();
  }
}

void rememberIdea() {  //function to play sound 2 hours after idea button was pressed
  if ((millis() - memTimerBegin) > (memTimerInterval) && startMemTimer == true)  //use millis to determine when to fade lights
  {
    for (int i = 0; i <= sizeof(memTones); i++) {
      CircuitPlayground.playTone(memTones[i], memTonesLength[i]);
      delay(memTonesLength[i] + 5);
    }
    startMemTimer = false;


  }
}


