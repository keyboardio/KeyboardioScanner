#ifndef __IS31IO7326_H__
#define __IS31IO7326_H__

#include "Arduino.h"

typedef struct {
  byte ad01;
  boolean down;
  byte key;
} key_t;

// this is the main method you want, to read a key from any configured controller
key_t waitForKey();


#define IS31IO7326_I2C_ADDR_BASE 0x58

// config options
#define AUTO_CLEAR_INT_DISABLED 0
#define AUTO_CLEAR_INT_5MS 0x20
#define AUTO_CLEAR_INT_10MS 0x40

#define INPUT_PORT_FILTER_ENABLE 0x10
#define INPUT_PORT_FILTER_DISABLE 0

#define KEY_SCAN_DEBOUNCE_TIME_DOUBLE_6_8MS 0
#define KEY_SCAN_DEBOUNCE_TIME_NORMAL_3_4MS 0x8

#define LONGPRESS_DETECT_ENABLE 0x4
#define LONGPRESS_DETECT_DISABLE 0

#define LONGPRESS_DELAY_20MS 0
#define LONGPRESS_DELAY_40MS 1
#define LONGPRESS_DELAY_1S 2
#define LONGPRESS_DELAY_2S 3

// used to configure interrupts, configuration for a particular controller
class Is7326 {
public:
  Is7326(byte setAd01);
  ~Is7326();
  void start(byte interrupt, byte pin);
  byte setConfig(byte config);
  byte setConfigOnce(byte config);
  int readConfig();
  void loopStart();
  void keyDown();

  //static key_t waitForKey();

private:
  bool configured = false;
  byte addr;
  byte ad01;
};

Is7326::~Is7326() {}

Is7326::Is7326(byte setAd01) {
  ad01 = setAd01;
  addr = IS31IO7326_I2C_ADDR_BASE | ad01;
}

// returns 0 on success, otherwise the Wire.endTransmission code
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte Is7326::setConfigOnce(byte config) {
  if (!configured) {
    setConfig(config);
  } else {
    return 0;
  }
}

// returns the Wire.endTransmission code (0 = success)
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte Is7326::setConfig(byte config) {
  Wire.beginTransmission(addr);
  Wire.write(0x8);
  Wire.write(config);
  return Wire.endTransmission();
}

// returns -1 on error, otherwise returns the 8 register configuration
int Is7326::readConfig() {
  Wire.beginTransmission(addr);
  Wire.write(0x8);
  byte error = Wire.endTransmission();
  if (error != 0) {
    return -1;
  }

  byte bytes = Wire.requestFrom(addr, 1, true);
  if (bytes != 1) {
    return -1;
  }
  return Wire.read();
}

// we have to hardcode the interrupt handlers as plain functions --
// C++ methods are not supported.
// Luckily, there can only be four I2C addresses for the controller.
// Use a 4-bit mask to tell us which controllers need to be read.
volatile byte keyReady = 0;
// Use a 4-bit mask to tell the interrupts that the key was read to
// help with debouncing.
volatile byte keyWasRead = 0xf;

#define KEY_DOWN_0 1
#define KEY_DOWN_1 2
#define KEY_DOWN_2 4
#define KEY_DOWN_3 8


void keyDown0() {
  if (keyWasRead & KEY_DOWN_0) {
    keyReady |= KEY_DOWN_0;
    keyWasRead &= ~KEY_DOWN_0;
  }
}
void keyDown1() {
  if (keyWasRead & KEY_DOWN_1) {
    keyReady |= KEY_DOWN_1;
    keyWasRead &= ~KEY_DOWN_1;
  }
}
void keyDown2() {
  if (keyWasRead & KEY_DOWN_2) {
    keyReady |= KEY_DOWN_2;
    keyWasRead &= ~KEY_DOWN_2;
  }
}
void keyDown3() {
  if (keyWasRead & KEY_DOWN_3) {
    keyReady |= KEY_DOWN_3;
    keyWasRead &= ~KEY_DOWN_3;
  }
}

void (*keyDowns[4])() = {keyDown0, keyDown1, keyDown2, keyDown3};

// attaches the proper interrupt controller to the given PIN
void Is7326::start(byte interruptPin, byte pin) {
  attachInterrupt(interruptPin, keyDowns[ad01], FALLING);
}

// returns the raw key code from the controller, or -1 on failure.
int readKey(byte ad01) {
  byte addr = IS31IO7326_I2C_ADDR_BASE | ad01;
  Wire.beginTransmission(addr);
  Wire.write(0x10);
  byte error = Wire.endTransmission();
  if (error != 0) {
    return -1;
  }

  byte bytes = Wire.requestFrom(addr, 1, true);
  if (bytes != 1) {
    return -1;
  }

  int d = Wire.read();
  return d;
}

// blocks and waits for a key to be pressed, and returns details about it.
key_t waitForKey() {
  int k = -1;
  byte bit = 0;
  byte ad01 = 0;
  while (k < 0) {
    while (keyReady == 0) {
      delay(1);
    }

    // read one key at a time
    if (keyReady & KEY_DOWN_0) {
      ad01 = 0;
      bit = KEY_DOWN_0;
    } else if (keyReady & KEY_DOWN_1) {
      ad01 = 1;
      bit = KEY_DOWN_1;
    } else if (keyReady & KEY_DOWN_2) {
      ad01 = 2;
      bit = KEY_DOWN_2;
    } else {
      ad01 = 3;
      bit = KEY_DOWN_3;
    }

    k = readKey(ad01);
    // if k < 0, we try again
  }

  // no extra keys, clear the interrupt flag
  if ((k & 0x80) == 0) {
    noInterrupts();
    keyReady &= ~bit;
    keyWasRead |= bit;
    interrupts();
  }
  key_t key;
  key.ad01 = ad01;
  key.down = (k & (1 << 6)) != 0;
  key.key = k & 0x1f;
  return key;
}

#endif