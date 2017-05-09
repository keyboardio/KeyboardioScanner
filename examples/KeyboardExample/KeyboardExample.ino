#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);

    Serial.begin(9600);
    TWBR = 4;
}

void loop() {
    keydata_t keys;

    // If there's any change to the keyboard state, print out the result
    if (controller.readKeys()) {
        Serial.print(controller.getKeyData().all, BIN);
        Serial.println();
    }
}
