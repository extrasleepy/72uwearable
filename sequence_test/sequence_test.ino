#include <Adafruit_CircuitPlayground.h>   //required library to use module
int fadeRate = 1500;
int between = 15;

void setup() {
  CircuitPlayground.begin();     // Setup Circuit Playground library.
  CircuitPlayground.strip.setBrightness(150); //brightness is between 0 and 255
  Serial.begin(9600);     // Setup serial port.
}
void loop() {
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
