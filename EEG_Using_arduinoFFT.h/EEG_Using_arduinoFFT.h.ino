#include <arduinoFFT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define samples      128        // Must be a power of 2
#define sampleRate   128        // Hz, determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
int dataArray[samples];
double vReal[samples];             // Each value is a frequency bin, stores the amount of that frequency as an amplitude, bin-widrh = SAMPLING_FREQ/SAMPLES
double vImag[samples];
arduinoFFT FFT = arduinoFFT(vReal, vImag, dataArray, sampleRate);

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Stores the calculated average magnitude of each type of brain wave
double delta;
double theta;
double alpha;
double beta;
double gamma;

// Stores the bar graph hight for each type of brain wave
int deltaGraph;
int thetaGraph;
int alphaGraph;
int betaGraph;
int gammaGraph;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);
}

void loop() {

  // Reads and records 128 data points from analog pin 1 in 1 second
  for (int sample = 0; sample < sampleRate; sample++) {
    dataArray[sample] = analogRead(1);
    delay(round(1024 / sampleRate));
  }

  // Prints dataArray to the serial monitor
  for (int i = 0; i < sampleRate; i++) {
    Serial.println(dataArray[i]);
  }

  // Clalculates the FFT
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  // Analysis of the FFT
  for (int i = 2; i < (samples / 2); i++) {
    if (i <= 4) delta += vReal[i];
    if ((i > 4) && (i <= 8)) theta += vReal[i];
    if ((i > 8) && (i <= 12)) alpha += vReal[i];
    if ((i > 12) && (i <= 35)) beta += vReal[i];
    if ((i > 35) && (i <= 45)) gamma += vReal[i];
  }

  // Prints the brainwave values to to serial monitor for debugging purposes
  Serial.print(delta);
  Serial.print(" ");
  Serial.print(theta);
  Serial.print(" ");
  Serial.print(alpha);
  Serial.print(" ");
  Serial.print(beta);
  Serial.print(" ");
  Serial.println(gamma);

  // calculates and stores the bar graph hight for each type of brain wave
  deltaGraph = round(0.01 * delta);
  thetaGraph = round(0.01 * theta);
  alphaGraph = round(0.01 * alpha);
  betaGraph = round(0.01 * beta);
  gammaGraph = round(0.01 * gamma);

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
}

float average (uint32_t *Array, int count) {
  int sum = 0;
  for (int i = 0; i < count; i++) {
    sum += Array [i] ;
  }
  return ((float)sum / count);
}
