#include "arduinoFFT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

arduinoFFT FFT = arduinoFFT();

const uint16_t samples = 64;
const double sampling = 40;
const uint8_t amplitude = 4;
uint8_t exponent;
const double startFrequency = 2;
const double stopFrequency = 16.4;
const double step_size = 0.1;

double vReal[samples];
double vImag[samples];

unsigned long time;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03


void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
    display.clearDisplay();
    
}
