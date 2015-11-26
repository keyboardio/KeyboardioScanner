#ifndef __IS31IO7326_H__
#define __IS31IO7326_H__

#include <Arduino.h>

typedef struct {
  int ad01;
  boolean down; // TODO: Arduino booleans are big. we can make this struct a lot tighter.
  byte key;
} key_t;

// this is the main method. It reads a key event from the IO Expander
int readRawKey(int addr);

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
class KeyboardioScanner {
public:
  KeyboardioScanner(byte setAd01);
  ~KeyboardioScanner();
  byte setConfig(byte config);
  byte setConfigOnce(byte config);
  int readConfig();
  bool moreKeysWaiting();
  key_t readKey();


private:
  bool configured = false;
  int addr;
  int ad01;
  bool keyReady = false;

};


#endif