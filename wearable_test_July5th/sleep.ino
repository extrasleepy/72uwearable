void sleepyTime() {
  for (int i = 0; i < 7; i++) {
    CircuitPlayground.strip.setPixelColor(pixels[i], 0, 0, 0);   //all lights off
    CircuitPlayground.strip.show();  // update pixels!
  }

  xMove = CircuitPlayground.motionX();
  yMove = CircuitPlayground.motionY();
  zMove = CircuitPlayground.motionZ();

  if (xMove >= xPrevious - 1 && xMove <= xPrevious + 1 && yMove >= yPrevious - 1 && yMove <= yPrevious + 1 && zMove >= zPrevious - 1 && zMove <= zPrevious + 1) {
    Watchdog.sleep(2000);
  }
  else {
    moveTimer = 0;
  }
  xPrevious = xMove;
  yPrevious = yMove;
  zPrevious = zMove;
}
