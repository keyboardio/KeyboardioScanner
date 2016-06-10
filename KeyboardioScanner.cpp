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

// Returns the relative controller addresss. The expected range is 0-3
uint8_t KeyboardioScanner::controllerAddress() {
    return ad01;
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

    // read one key
    uint8_t bytes_returned = Wire.requestFrom(addr,4,false);

    if( bytes_returned) {
        Serial.print(bytes_returned);
        Serial.println(" bytes returned.");
        uint8_t data = 0;
        while(Wire.available()) {
            data= Wire.read();
            Serial.print(data, BIN);
            Serial.print(" ");
        }

        key.val = data;
    }

    // no extra keys, clear the keyReady flag for this address
    if (key.keyEventsWaiting == 0 ) {
        keyReady = false;
    } else {
        keyReady = true;
    }
    return key;
}



void KeyboardioScanner::sendLEDData() {
    sendLEDBank(nextLEDBank++);
    if (nextLEDBank == LED_BANKS) {
        nextLEDBank = 0;
    }
}



void KeyboardioScanner::sendLEDBank(byte bank) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_LED_BASE+bank);
    for (int i=0; i<LED_BYTES_PER_BANK; i++) {
        Wire.write(ledData.bytes[bank][i]);
    }
    Wire.endTransmission();
}


