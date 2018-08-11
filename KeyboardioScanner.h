#pragma once

#include <Arduino.h>
#include "wire-protocol-constants.h"

struct cRGB {
  uint8_t b;
  uint8_t g;
  uint8_t r;
};

#define LED_BANKS 4

#define LEDS_PER_HAND 32
#define LED_BYTES_PER_BANK sizeof(cRGB)  * LEDS_PER_HAND/LED_BANKS

typedef union {
  cRGB leds[LEDS_PER_HAND];
  byte bytes[LED_BANKS][LED_BYTES_PER_BANK];
} LEDData_t;


typedef union {
  uint8_t rows[4];
  uint32_t all;
} keydata_t;

// config options

// used to configure interrupts, configuration for a particular controller
class KeyboardioScanner {
 public:
  KeyboardioScanner(byte setAd01);
  ~KeyboardioScanner();

  int readVersion();

  byte setKeyscanInterval(byte delay);
  int readKeyscanInterval();

  byte setLEDSPIFrequency(byte frequency);
  int readLEDSPIFrequency();

  void sendLEDData();
  void setOneLEDTo(byte led, cRGB color);
  void setAllLEDsTo(cRGB color);
  keydata_t getKeyData();
  bool readKeys();
  LEDData_t ledData;
  uint8_t controllerAddress();

 private:
  int addr;
  int ad01;
  keydata_t keyData;
  byte nextLEDBank = 0;
  void sendLEDBank(byte bank);
  int readRegister(uint8_t cmd);
};

