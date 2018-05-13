#include <util/crc16.h>
#include <Arduino.h>
#include "KeyboardioScanner.h"
#include "KeyboardioScannerFirmware.h"


extern "C" {
#include "twi.h"
}

#define TWI_ENDTRANS_SUCCESS 0
#define TWI_ENDTRANS_DATA_TOO_LONG 1
#define TWI_ENDTRANS_ADDR_NACK 2
#define TWI_ENDTRANS_DATA_NACK 3
#define TWI_ENDTRANS_ERROR 4


#define SCANNER_I2C_ADDR_BASE 0x58
#define ELEMENTS(arr)  (sizeof(arr) / sizeof((arr)[0]))


#define debug_print(...) Serial.print(__VA_ARGS__)
#define debug_print_result(result) debug_print(F("(result = ")); debug_print(result,DEC); debug_print(F(")\n"));



uint8_t twi_uninitialized = 1;

const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
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

uint8_t KeyboardioScanner::controllerI2CAddress() {
  return addr;
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
    data[i + 1] = pgm_read_byte(&gamma8[ledData.bytes[bank][i]]);
  }
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);
}



void KeyboardioScanner::setAllLEDsTo(cRGB color) {
  uint8_t data[] = {TWI_CMD_LED_SET_ALL_TO,
                    pgm_read_byte(&gamma8[color.b]),
                    pgm_read_byte(&gamma8[color.g]),
                    pgm_read_byte(&gamma8[color.r])
                   };
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);
}

void KeyboardioScanner::setOneLEDTo(byte led, cRGB color) {
  uint8_t data[] = {TWI_CMD_LED_SET_ONE_TO,
                    led,
                    pgm_read_byte(&gamma8[color.b]),
                    pgm_read_byte(&gamma8[color.g]),
                    pgm_read_byte(&gamma8[color.r])
                   };
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);

}


uint8_t KeyboardioScanner::bootloaderReadCrc16(byte *version, uint16_t *crc16, uint16_t offset, uint16_t length) {
    uint8_t result = TWI_ENDTRANS_ADDR_NACK;


// get version and CRC16 // addr (lo) // addr (hi) // len (lo) // len (hi)
    uint8_t data[] = { (0x06), (uint8_t)(offset & 0xff), (uint8_t)(offset >> 8), (uint8_t)(length & 0xff), (uint8_t)(length >> 8) };
    result = twi_writeTo(addr, data, ELEMENTS(data), true, true);


    if (result != TWI_ENDTRANS_SUCCESS) {
        return result;
    }

    uint8_t rxBuffer[3];

    // perform blocking read into buffer
    uint8_t read = twi_readFrom(addr, rxBuffer, ELEMENTS(rxBuffer), true);
    if (read == TWI_ENDTRANS_SUCCESS) {
    }
    if (read < TWI_ENDTRANS_DATA_NACK) {
        return 0xFF;
    }
    uint8_t v = rxBuffer[0];
    *version = v;
    uint8_t crc16_lo = rxBuffer[1];
    uint8_t crc16_hi = rxBuffer[2];
    *crc16 = (crc16_hi << 8) | crc16_lo;
    return result;
}


void KeyboardioScanner::bootloaderGetVersion () {

    byte result = TWI_ENDTRANS_ADDR_NACK;
    while (result != TWI_ENDTRANS_SUCCESS) {
        debug_print(F("Reading CRC16: "));

        byte version;
        uint16_t crc16;
        result = bootloaderReadCrc16(&version, &crc16, 0, firmware_length);
        debug_print_result(result);

        if (result != TWI_ENDTRANS_SUCCESS) {
            _delay_ms(100);
            continue;
        }
        debug_print(F("Version: "));
        debug_print(version);
        debug_print(F("\nExisting CRC16 of 0000-1FFF: "));
        debug_print(crc16, HEX);
        debug_print(F("\n"));
    }

}


int KeyboardioScanner::bootloaderEraseProgram () {
    // erase user space
    uint8_t data[] = { 0x04 };
    uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), true, true);

    debug_print(F("Erasing: "));
    debug_print_result(result);
    if (result != TWI_ENDTRANS_SUCCESS) {
        _delay_ms(1000);
        debug_print(F("failed.\n"));
        return -1;
    }
    return 0;

}

int KeyboardioScanner::bootloaderWriteFirmware() {

    uint8_t result = TWI_ENDTRANS_DATA_NACK;
    uint8_t o = 0;

    for (uint16_t i = 0; i < firmware_length; i += page_size) {
        debug_print(F("Page "));
        debug_print(offsets[o]);
        debug_print(F(" setting address "));

        // write page addr
        uint8_t data[] = { 0x01, (uint8_t)(offsets[o] & 0xff), (uint8_t)(offsets[o] >> 8)};
        result = twi_writeTo(addr, data, ELEMENTS(data), true, true);
        debug_print_result(result);

        _delay_ms(DELAY);
        // got something other than ACK. Start over.
        if (result != TWI_ENDTRANS_SUCCESS) {
            debug_print(F("\nFailed\n"));
            return -1;
        }

        // transmit each frame separately

        for (uint8_t frame = 0; frame < page_size / frame_size; frame++) {
            uint8_t data_counter =0;
            uint8_t data[frame_size +4] = {0};
            data[data_counter++] = 0x2; // continue page
            uint16_t crc16 = 0xffff;
            for (uint8_t j = frame * frame_size; j < (frame + 1) * frame_size; j++) {
                if (i + j < firmware_length) {
                    uint8_t b = pgm_read_byte(&firmware[i + j]);
                    data[data_counter++] = b;
                    crc16 = _crc16_update(crc16, b);
                } else {
                    data[data_counter++] = blank;
                    crc16 = _crc16_update(crc16, blank);
                }
            }
            // write the CRC16, little end first
            data[data_counter++] = (uint8_t)(crc16 & 0xff);
            data[data_counter++] = (uint8_t)(crc16 >> 8);
            data[data_counter++] = (0x00); // dummy end uint8_t

            result = twi_writeTo(addr, data, ELEMENTS(data), true, true);
            debug_print(F(",Frame "));
            debug_print(frame);
            debug_print_result(result);
            // got something other than NACK. Start over.
            if (result != TWI_ENDTRANS_DATA_NACK) {
                debug_print(F("\nERROR: Got something other than NACK\n") );
                return -1;
            }
            delay(DELAY);
        }
        debug_print(F("\n"));
        o++;
    }
    return 0;
}

int KeyboardioScanner::bootloaderVerifyFirmware() {
    byte result = TWI_ENDTRANS_DATA_NACK;
    // verify firmware
    debug_print(F("Verifying install\n"));
    while (result != TWI_ENDTRANS_SUCCESS) {
        debug_print(F("CRC16 "));

        byte version;
        uint16_t crc16;
        // skip the first 4 bytes, are they were probably overwritten by the reset vector preservation
        result = bootloaderReadCrc16(&version, &crc16, offsets[0] + 4, firmware_length - 4);

        debug_print(result);

        if (result != TWI_ENDTRANS_SUCCESS) {
            _delay_ms(100);
            continue;
        }
        debug_print(F("Version: "));
        debug_print(version);
        debug_print(F("\nCRC CRC16 of "));
        debug_print(offsets[0] + 4, HEX);
        debug_print(F("-"));
        debug_print(offsets[0] + firmware_length, HEX);
        debug_print(F(": "));
        debug_print(crc16, HEX);
        debug_print(F("\n"));
        // calculate our own CRC16
        uint16_t check_crc16 = 0xffff;
        for (uint16_t i = 4; i < firmware_length; i++) {
            check_crc16 = _crc16_update(check_crc16, pgm_read_byte(&firmware[i]));
        }
        if (crc16 != check_crc16) {
            debug_print(F("does not match: "));
            debug_print(check_crc16, HEX);
            return -1;
        }
        debug_print(F("OK\n"));
    }
    return 0;
}

int KeyboardioScanner::bootloaderUpdateAttiny() {
    debug_print(F("Communicating\n"));

    bootloaderGetVersion();

    int erased = bootloaderEraseProgram();

    if (erased == -1) {

        return 0;
    }

    int firmware_written = bootloaderWriteFirmware();
    if(firmware_written == -1) {
        debug_print(F("Write failed.\n"));
        return 0;
    }

    int firmware_verified = bootloaderVerifyFirmware();
    if(firmware_verified == -1) {
        debug_print(F("Verify failed.\n"));
        return 0;
    }

    debug_print(F("Resetting "));
    // execute app
    uint8_t data[] = {0x03, 0x00};
    uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), true, true);

    debug_print_result(result);
    debug_print(F("Done!\n"));

    return 1;
}

