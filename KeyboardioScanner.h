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


// Same datastructure as on the other side
typedef union {
    struct {
        uint8_t row:2,
                col:3,
                keyState:1,
                keyEventsWaiting:1,
                eventReported:1;
    };
    uint8_t val;
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
    uint8_t readKeyRaw();
    key_t readKey();
    LEDData_t ledData;
    uint8_t controllerAddress();

  private:
    bool configured = false;
    int addr;
    int ad01;
    bool keyReady = false;
    byte nextLEDBank = 0;
    void sendLEDBank(byte bank);
};

