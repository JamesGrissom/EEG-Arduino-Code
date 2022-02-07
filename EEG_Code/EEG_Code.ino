#include "arduinoFFT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

}

void loop() {
  
  // Stores the calculated power of each type of brain wave
  double delta = 0;
  double theta = 0;
  double aplpha = 0;
  double beta = 0;
  double gamma = 0;
  
  // Stores the bar graph hight for each type of brain wave
  int deltaGraph = 10;
  int thetaGraph = 20;
  int alphaGraph = 30;
  int betaGraph = 40;
  int gammaGraph = 50;
  
  display.clearDisplay();

  // Diplsys Column Labels for the bar graph
  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(8, 0);
  display.print("D");
  display.setCursor(34, 0);
  display.print("T");
  display.setCursor(60, 0);
  display.print("A");
  display.setCursor(86, 0);
  display.print("B");
  display.setCursor(112, 0);
  display.print("G");

  // Displays the data in a bar graph
  display.fillRect(0, (64 - deltaGraph), 24, deltaGraph, WHITE);
  display.fillRect(26, (64 - thetaGraph), 24, thetaGraph, WHITE);
  display.fillRect(52, (64 - alphaGraph), 24, alphaGraph, WHITE);
  display.fillRect(78, (64 - betaGraph), 24, betaGraph, WHITE);
  display.fillRect(104, (64 - gammaGraph), 24, gammaGraph, WHITE);
  
  display.display();

}
