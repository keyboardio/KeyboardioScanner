#include <Wire.h>
#include <KeyboardioScanner.h>

KeyboardioScanner controller(0);

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  // has to be done in the main loop, after everything is initialized
  controller.setConfigOnce(INPUT_PORT_FILTER_ENABLE |
                      KEY_SCAN_DEBOUNCE_TIME_NORMAL_3_4MS);

  // check if a key is ready, and if so, then read it.
  if (isKeyReady()) {
    key_t k = readKey();

    Serial.print("Configuration: ");
    Serial.print(controller.readConfig(), BIN);
    Serial.print("\n\n");

    Serial.print("Controller ");
    Serial.print(k.ad01, BIN);
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
