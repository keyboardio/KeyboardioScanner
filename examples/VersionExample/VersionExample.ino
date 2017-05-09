#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);

    Serial.begin(9600);
    delay(5000);
    Serial.print("Talking to an ATTiny running scanner firmware version ");
    Serial.print(controller.readVersion());
    Serial.println();
}

void loop() {
    1;
}
