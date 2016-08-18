#include <Wire.h>
#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

cRGB red;
cRGB blue;


void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

blue.b=255;
red.r=255;
  Wire.begin();
}

void loop() {
 controller.setAllLEDsTo(red);
  delay(1000);
 controller.setAllLEDsTo(blue);
 delay(1000);
 controller.disableLEDs();
 delay(1000);

  controller.setOneLEDTo(0,red);
  controller.setOneLEDTo(31,blue);
  delay(1000);
}
