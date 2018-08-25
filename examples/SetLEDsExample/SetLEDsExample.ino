/* KeyboardioScanner
 * Copyright (C) 2015-2018  Keyboard.io, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

cRGB red;
cRGB blue;
cRGB off;


void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  blue.b = 255;
  red.r = 255;
  off.r = off.g = off.b = 0;
}

void loop() {
  controller.setAllLEDsTo(red);
  delay(1000);
  controller.setAllLEDsTo(blue);
  delay(1000);
  controller.setAllLEDsTo(off);
  delay(1000);

  controller.setOneLEDTo(0, red);
  controller.setOneLEDTo(31, blue);
  delay(1000);
}
