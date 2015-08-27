#include <Wire.h>
#include "is31io7326.h"

Is7326 ctrl0(0);
// Is7326 ctrl0(1); // if you have AD1, AD0 = 0, 1 for a second controller

void setup() {
  Wire.begin();
  ctrl0.start(3, 1); // interrupt, PIN -- see https://www.arduino.cc/en/Reference/AttachInterrupt
  //ctrl1.start(1, 2);
}

void loop() {
  // has to be done in the main loop, after everything is initialized
  ctrl0.setConfigOnce(AUTO_CLEAR_INT_5MS |
                      INPUT_PORT_FILTER_ENABLE |
                      KEY_SCAN_DEBOUNCE_TIME_DOUBLE_6_8MS | // this is the most important bit
                      LONGPRESS_DETECT_DISABLE |
                      LONGPRESS_DELAY_20MS);
  // ctrl1.setConfigOnce(AUTO_CLEAR_INT_5MS |
  //                     INPUT_PORT_FILTER_ENABLE |
  //                     KEY_SCAN_DEBOUNCE_TIME_DOUBLE_6_8MS |
  //                     LONGPRESS_DETECT_DISABLE |
  //                     LONGPRESS_DELAY_20MS);

  Serial.print("Configuration: ");
  Serial.print(ctrl0.readConfig(), BIN);
  Serial.print("\n\n");

  Serial.print("Waiting for key\n");
  // main method to call
  // this will wait for a key to be pressed on any controller
  key_t k = waitForKey();

  Serial.print("Controller ");
  Serial.print(k.ad01, BIN);
  if (k.down) {
    Serial.print("  pressed key ");
  } else {
    Serial.print(" released key ");

  }
  Serial.print(k.key, BIN);
  Serial.print("\n");
  delay(1);
}