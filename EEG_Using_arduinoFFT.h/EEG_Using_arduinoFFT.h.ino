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

// On an Arduino:      A4(SDA), A5(SCL)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Stores the scaled brainwave power to be displayed in a bar graph
int waveGraph;

// Function Declarations
void clearGraph();
double averagePower(double realArray[], double imaginaryArray[], int start, int finish);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);

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

  // Clears the graph so it can display the new set of data
  clearGraph();

  // Analysis of the FFT, the numbers represent the brainwave frequency ranges

  // Computes and displays the relative power of delta waves in the first bar of the bar graph
  waveGraph = (int)averagePower(vReal, vImag, 1, 4);
  display.fillRect(0, (64 - waveGraph), 24, waveGraph, WHITE);
  display.display();
  waveGraph = 0;  // Resets waveaGraph value so the variable can be reused for the next wave type

  // Computes and displays the relative power of theta waves in the second bar of the bar graph
  waveGraph = (int)averagePower(vReal, vImag, 4, 8);
  display.fillRect(26, (64 - waveGraph), 24, waveGraph, WHITE);
  display.display();
  waveGraph = 0;  // Resets waveaGraph value so the variable can be reused for the next wave type

  // Computes and displays the relative power of alpha waves in the third bar of the bar graph
  waveGraph = (int)averagePower(vReal, vImag, 8, 12);
  display.fillRect(52, (64 - waveGraph), 24, waveGraph, WHITE);
  display.display();
  waveGraph = 0;  // Resets waveaGraph value so the variable can be reused for the next wave type

  // Computes and displays the relative power of beta waves in the fourth bar of the bar graph
  waveGraph = (int)averagePower(vReal, vImag, 12, 35);
  display.fillRect(78, (64 - waveGraph), 24, waveGraph, WHITE);
  display.display();
  waveGraph = 0;  // Resets waveaGraph value so the variable can be reused for the next wave type

  // Computes and displays the relative power of gamma waves in the fifth bar of the bar graph
  waveGraph = (int)averagePower(vReal, vImag, 35, 45);
  display.fillRect(104, (64 - waveGraph), 24, waveGraph, WHITE);
  display.display();
  waveGraph = 0;  // Resets waveaGraph value so the variable can be reused for the next wave type
}

void clearGraph() {
  display.fillRect(0, 6, 128, 64, BLACK);
  display.display();
  return; 
}

double averagePower(double realArray[], double imaginaryArray[], int start, int finish) {
  double sum;
  int coefficientCount = finish - start;
  for (start; start < finish; start++) {
    sum = (sq(realArray[start]) + sq(imaginaryArray[start]));
  }
  return sqrt(sum / coefficientCount);
}
