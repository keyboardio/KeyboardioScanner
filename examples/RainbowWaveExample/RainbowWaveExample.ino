#include <Wire.h>
#include <KeyboardioScanner.h>

    cRGB rainbow;

int rainbow_current_ticks= 0;
int rainbow_wave_ticks=1;
    uint16_t rainbow_hue = 0;   //stores 0 to 614

    uint16_t rainbow_wave_hue = 0;   //stores 0 to 614


  static const uint8_t rainbow_wave_steps=1;
KeyboardioScanner controller2(3);
KeyboardioScanner controller(0);


void led_set_crgb_at(int i, cRGB color) {
  if(i<32) {
      controller.ledData.leds[i] = color;
      //    controller2.ledData.leds[i] = color;

    
  } else {
    controller2.ledData.leds[i-32] = color;
  }
  
}
// From http://web.mit.edu/storborg/Public/hsvtorgb.c - talk to Scott about licensing
void hsv_to_rgb(cRGB *cRGB, uint16_t h, uint16_t s, uint16_t v)  {
    /* HSV to RGB conversion function with only integer
     * math */
    uint16_t region, fpart, p, q, t;

    if(s == 0) {
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
    switch(region) {
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
        hsv_to_rgb(&rainbow, rainbow_hue, 255, 255);
            for (uint8_t i = 0; i < 64; i++) {

        led_set_crgb_at(i,rainbow);
    }
   
}


void effect_rainbow_wave_update(int direction) {
    if (rainbow_current_ticks++ < rainbow_wave_ticks) {
        return;
    } else {
        rainbow_current_ticks = 0;
    }

    for (uint8_t i = 0; i < 64; i++) {
        uint16_t key_hue = rainbow_wave_hue +16*(i/4);
        if (key_hue >= 255)          {
            key_hue %= 255;
        }
        hsv_to_rgb(&rainbow, key_hue, 255, 255);
        led_set_crgb_at(i,rainbow);
    }
    rainbow_wave_hue += (rainbow_wave_steps * direction);
    if (rainbow_wave_hue >= 255)          {
        rainbow_wave_hue %= 255;
    } else if (rainbow_wave_hue < 0) {
     rainbow_wave_hue=255;
    }
}







void setup() {
    pinMode(13,OUTPUT);
    digitalWrite(13,HIGH);
    pinMode(7,OUTPUT);
   digitalWrite(7,LOW);

    Serial.begin(9600);
    Wire.begin();
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

int counter=0;
int next_led=0;
int8_t increment = 1;
void loop() {


for(auto i=0; i< 1000;i++) {

        effect_rainbow_wave_update(1);
        send_led_data();
}


for(auto i=0; i< 1000;i++) {

        effect_rainbow_wave_update(-1);
        send_led_data();
}

//for(auto i=0; i< 1000;i++) {
 //       effect_rainbow_update();
 //       send_led_data();
//}


//    key_t k = controller.readKey();
//    if (k.row[0]>0 ||k.row[1] >0||k.row[2]>0||k.row[3]>0 ) {
//        Serial.print("Controller ");
//        Serial.print(controller.controllerAddress(), DEC);
//        Serial.print(k.row[0] , BIN);
//        Serial.print(k.row[1] , BIN);
//        Serial.print(k.row[2] , BIN);
//        Serial.print(k.row[3] , BIN);
//        Serial.print("\n");
//    }
//
    delay(0);
}


















void set_all_leds_to(cRGB color) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        led_set_crgb_at(i, color);
    }
}




