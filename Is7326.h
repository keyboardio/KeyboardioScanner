#ifndef __IS31IO7326_H__
#define __IS31IO7326_H__

#include <Arduino.h>

typedef struct {
  byte ad01;
  boolean down;
  byte key;
} key_t;

// these are the two main methods -- one to check if a key has been
// pressed or released, and one to read it.
bool isKeyReady();
key_t readKey();

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
  void start(byte interrupt);
  byte setConfig(byte config);
  byte setConfigOnce(byte config);
  int readConfig();
  void keyDown();

private:
  bool configured = false;
  byte addr;
  byte ad01;
};


#endif