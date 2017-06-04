#include <KeyboardioScanner.h>

cRGB black;
cRGB rainbow;
int rainbow_current_ticks = 0;
int rainbow_wave_ticks = 1;
uint16_t rainbow_hue = 0;   // stores 0 to 614
uint16_t rainbow_wave_hue = 0;   // stores 0 to 614

static const uint8_t rainbow_wave_steps = 1;
KeyboardioScanner controller2(3);
KeyboardioScanner controller(0);


void led_set_crgb_at(int i, cRGB color) {
  if (i < 32) {
    controller.ledData.leds[i] = color;
    //    controller2.ledData.leds[i] = color;


  } else {
    controller2.ledData.leds[i - 32] = color;
  }
}
// From http://web.mit.edu/storborg/Public/hsvtorgb.c - talk to Scott about licensing
void hsv_to_rgb(cRGB *cRGB, uint16_t h, uint16_t s, uint16_t v)  {
  /* HSV to RGB conversion function with only integer
     math */
  uint16_t region, fpart, p, q, t;

  if (s == 0) {
    /* color is grayscale */
    cRGB->r = cRGB->g = cRGB->b = v;
    return;
  }

  /* make hue 0-5 */
  region = h / 43;
  /* find remainder part, make it from 0-255 */
  fpart = (h - (region * 43)) * 6;

  /* calculate temp vars, doing integer multiplication */
  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * fpart) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

  /* assign temp vars based on color cone region */
  switch (region) {
  case 0:
    cRGB->r = v;
    cRGB->g = t;
    cRGB->b = p;
    break;
  case 1:
    cRGB->r = q;
    cRGB->g = v;
    cRGB->b = p;
    break;
  case 2:
    cRGB->r = p;
    cRGB->g = v;
    cRGB->b = t;
    break;
  case 3:
    cRGB->r = p;
    cRGB->g = q;
    cRGB->b = v;
    break;
  case 4:
    cRGB->r = t;
    cRGB->g = p;
    cRGB->b = v;
    break;
  default:
    cRGB->r = v;
    cRGB->g = p;
    cRGB->b = q;
    break;
  }

  return;
}

void effect_rainbow_update() {
  rainbow_hue += 1;
  if (rainbow_hue >= 255)          {
    rainbow_hue %= 255;
  }
  hsv_to_rgb(&rainbow, rainbow_hue, 255, 10);
  controller.setAllLEDsTo(rainbow);
}

void effect_rainbow_wave_update(int direction) {
  if (rainbow_current_ticks++ < rainbow_wave_ticks) {
    return;
  } else {
    rainbow_current_ticks = 0;
  }

  for (uint8_t i = 0; i < 64; i++) {
    uint16_t key_hue = rainbow_wave_hue + 16 * (i / 4);
    if (key_hue >= 255)          {
      key_hue %= 255;
    }
    hsv_to_rgb(&rainbow, key_hue, 255, 100);
    led_set_crgb_at(i, rainbow);
  }
  rainbow_wave_hue += (rainbow_wave_steps * direction);
  if (rainbow_wave_hue >= 255)          {
    rainbow_wave_hue %= 255;
  }
  send_led_data();
}







void setup() {
  black.r = black.g = black.b = 0;
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  Serial.begin(9600);
  // TWBR=5;
}

void send_led_data() {
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
  controller2.sendLEDData();
  controller.sendLEDData();
}

int counter = 0;
int next_led = 0;
int8_t increment = 1;
void loop() {
  controller.setLEDSPIFrequency(LED_SPI_FREQUENCY_DEFAULT);

  for (auto i = 0; i < 255; i++) {
    effect_rainbow_update();
    delay(10);
  }

  controller.setLEDSPIFrequency(LED_SPI_FREQUENCY_64KHZ);

  for (auto i = 0; i < 255; i++) {
    effect_rainbow_wave_update(1);
  }

  controller.setLEDSPIFrequency(LED_SPI_FREQUENCY_1MHZ);

  for (auto i = 0; i < 255; i++) {
    effect_rainbow_wave_update(1);
  }


  controller.setAllLEDsTo(black);
  delay(10);
  controller.setLEDSPIFrequency(LED_SPI_OFF);
  delay(2000);

  controller.setLEDSPIFrequency(LED_SPI_FREQUENCY_1MHZ);

  for (auto i = 0; i < 5000; i++) {
    effect_rainbow_update();
  }
}

