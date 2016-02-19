#pragma once

#include <Arduino.h>


struct cRGB {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};


#define TWI_CMD_NONE 0x00
#define TWI_CMD_CFG 0x01
#define TWI_CMD_LED_BASE 0x80

#define LED_BANKS 8

#define LED_COUNT 32
#define LED_BYTES_PER_BANK sizeof(cRGB)  * LED_COUNT/LED_BANKS

typedef union LEDData_t {
    cRGB leds[LED_COUNT];
    byte bytes[LED_BANKS][LED_BYTES_PER_BANK];
};



typedef struct {
    int ad01;
    boolean down; // TODO: Arduino booleans are big. we can make this struct a lot tighter.
    byte key;
} key_t;

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
    void sendLEDData();
    key_t readKey();
    LEDData_t ledData;

  private:
    bool configured = false;
    int addr;
    int ad01;
    bool keyReady = false;
    byte nextLEDBank = 0;
    void sendLEDBank(byte bank);
};

