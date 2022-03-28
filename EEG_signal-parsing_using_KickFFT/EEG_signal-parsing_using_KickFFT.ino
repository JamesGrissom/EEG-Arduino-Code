#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "KickFFT.h"  // https://github.com/LinnesLab/KickFFT/tree/master/examples/EXAMPLE01_Basic

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

float sampleRate = 128;
uint16_t samples = 128;

float deltaLower = 0.5;
float deltaUpper = 4;

float thetaLower = 4.0001;
float thetaUpper = 8;

float alphaLower = 8.0001;
float alphaUpper = 12;

float betaLower = 12.0001;
float betaUpper = 35;

float gammaLower = 35.0001;
float gammaUpper = 45;

void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);
}

void loop() {

  int16_t dataArray[samples] = {10,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
75,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
110,
75,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
75,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
110,
75,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
75,
25,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
110,
75,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
50,
75,
5,
10,
50,
10,
25,
10,
50,
10,
5,
10,
50,
10,
25,
10,
128};
  uint32_t deltaMag[samples];
  uint32_t thetaMag[samples];
  uint32_t alphaMag[samples];
  uint32_t betaMag[samples];
  uint32_t gammaMag[samples];

  // Reads and records 128 data points from analog pin 1 in 1 second
//  for (int sample = 0; sample < sampleRate; sample++) {
//    dataArray[sample] = analogRead(1);
//    delay(round(1024 / sampleRate));
//  }
//
//  // Prints dataArray to the serial monitor
//  for (int i = 0; i < sampleRate; i++) {
//    Serial.println(dataArray[i]);
//  }

  // Determining the frequencies and magnitudes using KickFFT
  KickFFT<int16_t>::fft(sampleRate, deltaLower, deltaUpper, samples, dataArray, deltaMag);
//  KickFFT<int16_t>::fft(sampleRate, thetaLower, thetaUpper, samples, dataArray, thetaMag);
//  KickFFT<int16_t>::fft(sampleRate, alphaLower, alphaUpper, samples, dataArray, alphaMag);
//  KickFFT<int16_t>::fft(sampleRate, betaLower, betaUpper, samples, dataArray, betaMag);
//  KickFFT<int16_t>::fft(sampleRate, gammaLower, gammaUpper, samples, dataArray, gammaMag);

  // Stores the calculated average magnitude of each type of brain wave
  float delta = average(deltaMag, samples);
  float theta = average(thetaMag, samples);
  float alpha = average(alphaMag, samples);
  float beta = average(betaMag, samples);
  float gamma = average(gammaMag, samples);

  Serial.print(delta);
  Serial.print(" ");
  Serial.print(theta);
  Serial.print(" ");
  Serial.print(alpha);
  Serial.print(" ");
  Serial.print(beta);
  Serial.print(" ");
  Serial.println(gamma);
  
  
  // Stores the bar graph hight for each type of brain wave
  int deltaGraph = round(0.01 * delta);
  int thetaGraph = round(0.01 * theta);
  int alphaGraph = round(0.01 * alpha);
  int betaGraph = round(0.01 * beta);
  int gammaGraph = round(0.01 * gamma);

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
  display.display();

  // Displays the data in a bar graph
  display.fillRect(0, (64 - deltaGraph), 24, deltaGraph, WHITE);
  display.fillRect(26, (64 - thetaGraph), 24, thetaGraph, WHITE);
  display.fillRect(52, (64 - alphaGraph), 24, alphaGraph, WHITE);
  display.fillRect(78, (64 - betaGraph), 24, betaGraph, WHITE);
  display.fillRect(104, (64 - gammaGraph), 24, gammaGraph, WHITE);
  display.display();

  delay(1000);
}

float average (uint32_t *Array, int count) {
  int sum = 0;
  for (int i = 0; i < count; i++) {
    sum += Array [i] ;
  }
  return ((float)sum / count);
}
