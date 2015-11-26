#ifndef __IS31IO7326_H__
#define __IS31IO7326_H__

#include <Arduino.h>


#define TWI_CMD_NONE 0x00
#define TWI_CMD_ROWS 0x01
#define TWI_CMD_COLS 0x02
#define TWI_CMD_CFG  0x08

typedef struct {
  int ad01;
  boolean down; // TODO: Arduino booleans are big. we can make this struct a lot tighter.
  byte key;
} key_t;

// this is the main method. It reads a key event from the IO Expander
int readRawKey(int addr);

// config options

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