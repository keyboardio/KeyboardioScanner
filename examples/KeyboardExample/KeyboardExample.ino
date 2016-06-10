#include <Wire.h>
#include <KeyboardioScanner.h>

KeyboardioScanner controller(3);

void setup() {
    Serial.begin(9600);
    Wire.begin();
}

void loop() {

    // check if a key is ready, and if so, then read it.
    key_t k = controller.readKey();
    if (k.eventReported != 0) {
        Serial.print("Controller ");
        Serial.print(controller.controllerAddress(), DEC);
        if (k.keyState) {
            Serial.print("  pressed key ");
        } else {
            Serial.print(" released key ");

        }
        Serial.print(k.row , BIN);
        Serial.print(k.col , BIN);
        Serial.print("\n");
    }

    delay(1000);
    Serial.print(".");
}
