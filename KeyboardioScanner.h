#ifndef __KEYBOARDIO_SCANNER_H__
#define __KEYBOARDIO_SCANNER_H__

#include <Arduino.h>


#define TWI_CMD_NONE 0x00
#define TWI_CMD_LEDS 0x01
#define TWI_CMD_CFG  0x08
#define TWI_CMD_PREFIX_LED_BANK_0 0x20 // 0b00100000
#define TWI_CMD_PREFIX_LED_BANK_1 0x40 // 0b01000000
#define TWI_CMD_PREFIX_LED_BANK_2 0x60 // 0b01100000
#define TWI_CMD_PREFIX_LED_BANK_3 0x80 // 0b10000000
#define TWI_CMD_MASK_LED_BANK_0 0xC0   // 0b11000000 
#define TWI_CMD_MASK_LED_BANK_1 0xA0   // 0b10100000
#define TWI_CMD_MASK_LED_BANK_2 0x80   // 0b10000000
#define TWI_CMD_MASK_LED_BANK_3 0x60   // 0b01100000

#define LED_BANKS 4

typedef union {
    struct {
        uint8_t c;

        uint8_t b;
        uint8_t g;
        uint8_t r;
    };
    uint8_t array[4];
} cRGB;

#define LED_COUNT 32
typedef union LEDData_t {
    cRGB leds[LED_COUNT];
    byte bytes[LED_BANKS][sizeof(cRGB)*LED_COUNT/LED_BANKS];
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


#endif