#include <KeyboardioScanner.h>

cRGB black;
cRGB rainbow;
cRGB rainbow_out;
int rainbow_current_ticks = 0;
int rainbow_wave_ticks = 1;
uint16_t rainbow_hue = 0;   //stores 0 to 614

uint16_t rainbow_wave_hue = 0;   //stores 0 to 614

const uint8_t PROGMEM gamma[] = {
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

  // rainbow_out.r = pgm_read_byte(&gamma[rainbow.r]);
  //  rainbow_out.g = pgm_read_byte(&gamma[rainbow.g]);
  // rainbow_out.b = pgm_read_byte(&gamma[rainbow.b]);

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

    rainbow_out.r = pgm_read_byte(&gamma[rainbow.r]);
    rainbow_out.g = pgm_read_byte(&gamma[rainbow.g]);
    rainbow_out.b = pgm_read_byte(&gamma[rainbow.b]);

    led_set_crgb_at(i, rainbow_out);
  }
  rainbow_wave_hue += (rainbow_wave_steps * direction);
  if (rainbow_wave_hue >= 255)          {
    rainbow_wave_hue %= 255;
  } else if (rainbow_wave_hue < 0) {
    rainbow_wave_hue = 255;
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

