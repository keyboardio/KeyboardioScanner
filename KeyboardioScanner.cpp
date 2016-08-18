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


// Sets the key debouncing delay. The delay value will
// be multiplied by 20 and fed into _delay_us() on the 
// controller.
// 
// This gives you a maximum of 5100ms between reads of a
// given row's keys. We currently do three reads.
// You should think of this as the _minimum_ debounce time.
//
// The delay doesn't take into account time spent reading from
// I2C or updating LEDs. 
//
//
// returns the Wire.endTransmission code (0 = success)
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte KeyboardioScanner::setDebounceDelay(byte delay) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_DEBOUNCE_DELAY);
    Wire.write(delay);
    return Wire.endTransmission();
}




// returns -1 on error, otherwise returns the 8 register configuration
int KeyboardioScanner::readConfig() {
    return readRegister(TWI_CMD_CFG);
}

// returns -1 on error, otherwise returns the scanner version integer
int KeyboardioScanner::readVersion() {
    return readRegister(TWI_CMD_VERSION);
}

// returns -1 on error, otherwise returns the scanner version integer
int KeyboardioScanner::readDebounceDelay() {
    return readRegister(TWI_CMD_DEBOUNCE_DELAY);
}

int KeyboardioScanner::readRegister(int cmd) {

    byte return_value = 0;

    Wire.beginTransmission(addr);
    Wire.write(cmd);
    Wire.endTransmission();
    delayMicroseconds(15); // We may be able to drop this in the future
                           // but will need to verify with correctly
                           // sized pull-ups on both the left and right
                           // hands' i2c SDA and SCL lines
    Wire.requestFrom(addr, 1, true);
    return_value = Wire.read();
    return return_value;
}


// returns the raw key code from the controller, or -1 on failure.
// returns true of a key is ready to be read
bool KeyboardioScanner::moreKeysWaiting() {
    return keyReady;
}

// gives information on the key that was just pressed or released.
bool KeyboardioScanner::readKeys() {
    // read one key state
    Wire.requestFrom(addr,5,true);
   
    uint8_t event_detected = Wire.read();
    if (event_detected == TWI_REPLY_KEYDATA) {
        keyData.rows[0] = Wire.read();
        keyData.rows[1] = Wire.read();
        keyData.rows[2] = Wire.read();
        keyData.rows[3] = Wire.read();
         return true;
        } else {
            return false;
        }
}

keydata_t KeyboardioScanner::getKeyData() {
    return keyData;
}

void KeyboardioScanner::sendLEDData() {
    sendLEDBank(nextLEDBank++);
    if (nextLEDBank == LED_BANKS) {
        nextLEDBank = 0;
    }
}

void KeyboardioScanner::disableLEDs() {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_LED_DISABLE);
    Wire.endTransmission();
}


void KeyboardioScanner::sendLEDBank(byte bank) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_LED_BASE+bank);
    for (int i=0; i<LED_BYTES_PER_BANK; i++) {
        Wire.write(ledData.bytes[bank][i]);
    }
    Wire.endTransmission();
}



void KeyboardioScanner::setAllLEDsTo( cRGB color) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_LED_SET_ALL_TO);
    Wire.write(color.r);
    Wire.write(color.g);
    Wire.write(color.b);
    Wire.endTransmission();
}
void KeyboardioScanner::setOneLEDTo(byte led, cRGB color) {
    Wire.beginTransmission(addr);
    Wire.write(TWI_CMD_LED_SET_ONE_TO);
    Wire.write(led);
    Wire.write(color.r);
    Wire.write(color.g);
    Wire.write(color.b);
    Wire.endTransmission();
}


