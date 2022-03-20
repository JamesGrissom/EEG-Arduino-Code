#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "arduinoFFT.h"

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);
}

void loop() {
  const int sampleRate = 128;
  int dataArray[sampleRate];

  // Reads and records 128 data points from analog pin 1 in 1 second
  for (sample = 0, sample < sampleRate, sample++) {
    dataArray[sample] = analogRead(1);
    delay(round(1024 / sampleRate));
  }

  // Prints dataArray to the serial monitor
  for (int i = 0; i < sampleRate; i++) {
    Serial.println(dataArray[i]);
  }

  // Determining the frequencies using arduinoFFT


  double signalVoltage = analogRead(1) / 204.8;

  // Stores the calculated power of each type of brain wave
  double delta = signalVoltage;
  double theta = signalVoltage;
  double alpha = signalVoltage;
  double beta = signalVoltage;
  double gamma = signalVoltage;

  // Stores the bar graph hight for each type of brain wave
  int deltaGraph = round(11.6 * delta);
  int thetaGraph = round(11.6 * theta);
  int alphaGraph = round(11.6 * alpha);
  int betaGraph = round(11.6 * beta);
  int gammaGraph = round(11.6 * gamma);

  display.clearDisplay();

  // Diplays Column Labels for the bar graph
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

  sample = 0;
}
