//Code for 72u LOCU project. Summer 2016.

/* ToDo:
   ->Make tones more interesting
   ->Sensor numbers are calibrated but could use additional testing and adjustment

   Hardware:
   DONE -> Remove Battery connection from main circuit
   DONE -> Remove or break power LED
   DONE -> Battery goes from breakout board BATT to CirPlay VBATT
*/

#include <Adafruit_CircuitPlayground.h>   //required library to use module
#include <Adafruit_SleepyDog.h>  //library that allows low power sleeping

#define TEMP A0   //Analog 0 is connected to temperature sensor
#define SOUND A4  //Analog 4 is connected to sound sensor/microphone
#define LIGHT A5  //Analog 5 is connected to light sensor

#define CAP_THRESHOLD    50  // Threshold for a capacitive touch (higher = less sensitive).
#define CAP_SAMPLES      20   // Number of samples to take for a capacitive touch read.

float tMin = 70;   //min max variables for sensors temp = 70-74 sound=300-550 light=20-120
float tMax = 74;   //all these numbers could use some testing
float sMin = 300;
float sMax = 550;
float lMin = 10;
float lMax = 120;

uint8_t xPrev = 0;  //variables to test movement for sleep mode
uint8_t yPrev = 0;
uint8_t zPrev = 0;
uint8_t xMove = 0;
uint8_t yMove = 0;
uint8_t zMove = 0;

int moveFlex = 2; //a little flexibility for minor vibrations
int moveTimer = 0;  //timer keeps track how long since last movement
int verySleepy = 1400; //1 represents about 1 second + total light fade times (1400 = 30ish min)

int pixels[] = {0, 2, 4, 5, 7, 9};   //array of used light pins
int perfectTones[] =       {300, 380, 440, 620};   //array for perfect environment sound
int perfectTonesLength[] = {125, 125, 200, 300};  //array for adding sound durations

int memTones[] =       {880, 620, 480, 520, 480, 880, 780}; //array for idea button tones
int memTonesLength[] = {125, 125, 125, 125, 250, 125, 250}; //array for idea button tone lengths
bool startMemTimer = false;  //used for idea button
long memTimerInterval = 3500000;  //time before idea reminder tone (about 2 hours)
long memTimerBegin = 0;  //timer keeps track how long since idea button pressed

bool resetSpin = true;  //keeps track of how often light spin happens

long fadeInterval = 60000; // interval between light fades (60ish seconds)
long sinceLastFade = 59000; //fades one time right away when powered up

uint16_t tempValue = 0;   //used to average sensors
uint16_t soundValue = 0;
uint16_t lightValue = 0;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(170); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.
}

void loop() {
  ideaButton();   //check idea button state
  rememberIdea();  //play idea reminder if enough time has passed

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

//function to control lights and sound based on sensor readings
uint16_t lightUp(uint16_t tempValue, uint16_t soundValue, uint16_t lightValue) {

  //play tone if environment is perfect
  if (tempValue > tMin && tempValue < tMax && soundValue > sMin && soundValue < sMax && lightValue > lMin && lightValue < lMax && resetSpin == true) {
    for (int i = 0; i < sizeof(perfectTones)/sizeof(int); i++) {
      CircuitPlayground.playTone(perfectTones[i], perfectTonesLength[i]);
      delay(perfectTonesLength[i]);
    }

    //light spin twice
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
    } else {
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
        CircuitPlayground.strip.show();                                 // update pixels!
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

//function to save power when LOCU is at rest for specified time
void sleepyTime() {
  CircuitPlayground.clearPixels();

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  if (xMove >= xPrev - moveFlex && xMove <= xPrev + moveFlex && yMove >= yPrev - moveFlex && yMove <= yPrev + moveFlex && zMove >= zPrev - moveFlex && zMove <= zPrev + moveFlex) {
    Watchdog.sleep(1000);    //save power for 1 second
  } else {
    moveTimer = 0;
  }

  xPrev = xMove;
  yPrev = yMove;
  zPrev = zMove;

  ideaButton(); rememberIdea();
}

//checks to see if idea button had been pressed - touch capacitive touch pad 9 when you have a good idea
void ideaButton() {
  
  boolean memButton = CircuitPlayground.rightButton();

  if (CircuitPlayground.readCap(9, CAP_SAMPLES) >= CAP_THRESHOLD) {
      Serial.println( sizeof(memTones)/sizeof(int) );
      for (int i = 0; i < sizeof(memTones)/sizeof(int); i++) {
        CircuitPlayground.playTone(memTones[i], memTonesLength[i]);
        delay(memTonesLength[i]);
      }
      startMemTimer = true;
      memTimerBegin = millis();
    }
}

//plays idea button reminder if enough time had passed - 2 hours after idea button was pressed
void rememberIdea() {
  if ((millis() - memTimerBegin) > (memTimerInterval) && startMemTimer == true)  //use millis to determine when to play tone
  {
    for (int i = 0; i < sizeof(memTones)/sizeof(int); i++) {
      CircuitPlayground.playTone(memTones[i], memTonesLength[i]);
      delay(memTonesLength[i]);
    }
    startMemTimer = false;
  }
}


