#include <Arduino.h>
#include "KeyboardioScanner.h"

extern "C" {
#include "twi.h"
}


#define SCANNER_I2C_ADDR_BASE 0x58
#define ELEMENTS(arr)  (sizeof(arr) / sizeof((arr)[0]))

uint8_t twi_uninitialized = 1;

const uint8_t PROGMEM gamma8[] = {
  0,   0,   0,   0,
  0,   0,   0,   1,
  1,   1,   2,   2,
  3,   3,   4,   5,
  6,   7,   8,   10,
  11,  13,  14,  16,
  18,  20,  22,  25,
  27,  30,  33,  36,
  39,  43,  47,  50,
  55,  59,  63,  68,
  73,  78,  83,  89,
  95,  101, 107, 114,
  120, 127, 135, 142,
  150, 158, 167, 175,
  184, 193, 203, 213,
  223, 233, 244, 255
};

KeyboardioScanner::~KeyboardioScanner() {}

KeyboardioScanner::KeyboardioScanner(byte setAd01) {
  ad01 = setAd01;
  addr = SCANNER_I2C_ADDR_BASE | ad01;
  if (twi_uninitialized--) {
    twi_init();
  }
}

// Returns the relative controller addresss. The expected range is 0-3
uint8_t KeyboardioScanner::controllerAddress() {
  return ad01;
}

// Sets the keyscan interval. We currently do three reads.
// before declaring a key event debounced.
//
// Takes an integer value representing a counter.
//
// 0 - 0.1-0.25ms
// 1 - 0.125ms
// 10 - 0.35ms
// 25 - 0.8ms
// 50 - 1.6ms
// 100 - 3.15ms
//
// You should think of this as the _minimum_ keyscan interval.
// LED updates can cause a bit of jitter.
//
// returns the Wire.endTransmission code (0 = success)
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte KeyboardioScanner::setKeyscanInterval(byte delay) {
  uint8_t data[] = {TWI_CMD_KEYSCAN_INTERVAL, delay};
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);

  return result;
}




// returns -1 on error, otherwise returns the scanner version integer
int KeyboardioScanner::readVersion() {
  return readRegister(TWI_CMD_VERSION);
}

// returns -1 on error, otherwise returns the scanner keyscan interval
int KeyboardioScanner::readKeyscanInterval() {
  return readRegister(TWI_CMD_KEYSCAN_INTERVAL);
}


// returns -1 on error, otherwise returns the LED SPI Frequncy
int KeyboardioScanner::readLEDSPIFrequency() {
  return readRegister(TWI_CMD_LED_SPI_FREQUENCY);
}

// Set the LED SPI Frequency. See wire-protocol-constants.h for
// values.
//
// returns the Wire.endTransmission code (0 = success)
// https://www.arduino.cc/en/Reference/WireEndTransmission
byte KeyboardioScanner::setLEDSPIFrequency(byte frequency) {
  uint8_t data[] = {TWI_CMD_LED_SPI_FREQUENCY, frequency};
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);

  return result;
}



int KeyboardioScanner::readRegister(uint8_t cmd) {

  byte return_value = 0;

  uint8_t data[] = {cmd};
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);



  delayMicroseconds(15); // We may be able to drop this in the future
  // but will need to verify with correctly
  // sized pull-ups on both the left and right
  // hands' i2c SDA and SCL lines

  uint8_t rxBuffer[1];

  // perform blocking read into buffer
  uint8_t read = twi_readFrom(addr, rxBuffer, ELEMENTS(rxBuffer), true);
  if (read > 0) {
    return rxBuffer[0];
  } else {
    return -1;
  }

}


// gives information on the key that was just pressed or released.
bool KeyboardioScanner::readKeys() {

  uint8_t rxBuffer[5];

  // perform blocking read into buffer
  uint8_t read = twi_readFrom(addr, rxBuffer, ELEMENTS(rxBuffer), true);
  if (rxBuffer[0] == TWI_REPLY_KEYDATA) {
    keyData.rows[0] = rxBuffer[1];
    keyData.rows[1] = rxBuffer[2];
    keyData.rows[2] = rxBuffer[3];
    keyData.rows[3] = rxBuffer[4];
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

void KeyboardioScanner::sendLEDBank(byte bank) {
  uint8_t data[LED_BYTES_PER_BANK + 1];
  data[0]  = TWI_CMD_LED_BASE + bank;
  for (uint8_t i = 0 ; i < LED_BYTES_PER_BANK; i++) {
    data[i + 1] = pgm_read_byte(&gamma8[ledData.bytes[bank][i] >> 2]);
  }
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);
}



void KeyboardioScanner::setAllLEDsTo(cRGB color) {
  uint8_t data[] = {TWI_CMD_LED_SET_ALL_TO,
                    pgm_read_byte(&gamma8[color.b >> 2]),
                    pgm_read_byte(&gamma8[color.g >> 2]),
                    pgm_read_byte(&gamma8[color.r >> 2])
                   };
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);
}

void KeyboardioScanner::setOneLEDTo(byte led, cRGB color) {
  uint8_t data[] = {TWI_CMD_LED_SET_ONE_TO,
                    led,
                    pgm_read_byte(&gamma8[color.b >> 2]),
                    pgm_read_byte(&gamma8[color.g >> 2]),
                    pgm_read_byte(&gamma8[color.r >> 2])
                   };
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);

}


