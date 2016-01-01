#include <Arduino.h>
#include <Wire.h>
#include "KeyboardioScanner.h"

#define IS31IO7326_I2C_ADDR_BASE 0x58


KeyboardioScanner::~KeyboardioScanner() {}

KeyboardioScanner::KeyboardioScanner(byte setAd01) {
    ad01 = setAd01;
    addr = IS31IO7326_I2C_ADDR_BASE | ad01;
    // keyReady will be true after a read when there's another key event
    // already waiting for us
    keyReady = false;
}

// returns 0 on success, otherwise the Wire.endTransmission code
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte KeyboardioScanner::setConfigOnce(byte config) {
    if (!configured) {
        setConfig(config);
    } else {
        return 0;
    }
}

// returns the Wire.endTransmission code (0 = success)
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte KeyboardioScanner::setConfig(byte config) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_CFG);
    Wire.write(config);
    configured = true;
    return Wire.endTransmission();
}







// returns -1 on error, otherwise returns the 8 register configuration
int KeyboardioScanner::readConfig() {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_CFG);
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

// returns the raw key code from the controller, or -1 on failure.
// returns true of a key is ready to be read
bool KeyboardioScanner::moreKeysWaiting() {
    return keyReady;
}

// gives information on the key that was just pressed or released.
key_t KeyboardioScanner::readKey() {
    key_t key;
    int k = -1;

    // read one key
    if( Wire.requestFrom(addr, 1, true) ==1) {
        k = Wire.read();
    }

    // no extra keys, clear the keyReady flag for this address
    if ((k & 0x80) == 0) {
        keyReady = false;
    } else {
        keyReady = true;
    }

    key.ad01 = ad01;
    key.down = (k & (1 << 6)) != 0;
    key.key = k & 0x1f;
    return key;
}



void KeyboardioScanner::sendLEDData() {
    sendLEDBank(nextLEDBank++);
    if (nextLEDBank == 4) {
        nextLEDBank = 0;
    }
}



void KeyboardioScanner::sendLEDBank(byte bank) {
    Wire.beginTransmission(addr);
    for (int i=0; i<32; i++) {
        if(i==0) {
            byte data = ledData.bytes[bank][i];
            if(bank ==0) {
                data = data ^ TWI_CMD_MASK_LED_BANK_0;
            } else if (bank == 1) {
                data = data ^ TWI_CMD_MASK_LED_BANK_1;
            } else if (bank == 2) {
                data = data ^ TWI_CMD_MASK_LED_BANK_2;
            } else if (bank == 3) {
                data = data ^ TWI_CMD_MASK_LED_BANK_3;
            }
            Wire.write(data);
        } else {
            Wire.write(ledData.bytes[bank][i]);
        }
    }
    Wire.endTransmission();
}


