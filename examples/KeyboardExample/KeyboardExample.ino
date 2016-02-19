#include <Wire.h>
#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

void setup() {
    Serial.begin(9600);
    Wire.begin();
}

void loop() {

    // check if a key is ready, and if so, then read it.
    key_t k = controller.readKey();
    if (k.key>0) {
        Serial.print("Controller ");
        Serial.print(k.ad01, DEC);
        if (k.down) {
            Serial.print("  pressed key ");
        } else {
            Serial.print(" released key ");

        }
        Serial.print(k.key, BIN);
        Serial.print("\n");
    }

    delay(1);
}
