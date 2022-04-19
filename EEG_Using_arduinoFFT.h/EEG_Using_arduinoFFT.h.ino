// EEG using the arduinoFFT.h library
// Developed by James Grissom for use in ECE 2804 Integrated Design Project
// Completed in April of 2022

#include <arduinoFFT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define samples      128        // Must be a power of 2
#define sampleRate   128        // Hz, determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2
double vReal[samples];          // Each value is a frequency bin, stores the amount of that frequency as an amplitude; bin-width = SAMPLING_FREQ/SAMPLES
double vImag[samples];          // Stores the imaginary phase information about the input signal stored in vReal
int bargraphArray[5];           // Stores the average power of each of the 5 brain wave frequency ranges to be displayed on the small OLED screen
int divider = 0;                // Divides the data in the bargraphArray to scale it to properly fit the small OLED display
int Vmax = 0;
int Fmax = 0;
int Bmax = 0;

arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, sampleRate);

// On an Arduino:      A4(SDA), A5(SCL)
// On esp32 devkitc v4 21(SDA), 22(SCL)

// Sensor pins:
int ArduinoSensor = 1;
int esp32Sensor = 26;

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// averagePower function Declaration
double averagePower(double realArray[], int start, int finish);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(ArduinoSensor, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Reads and records 128 data points from analog pin 1 in 1 second
  for (int sample = 0; sample < sampleRate; sample++) {
    vReal[sample] = analogRead(ArduinoSensor);
    vImag[sample] = 0;
    delay(round(1000 / sampleRate));
  }

  // Calculates the FFT
  FFT.DCRemoval();    // Removes any DC offset from the recorded data
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);    // Sets the windowing for the FFT
  FFT.Compute(FFT_FORWARD);   // Computes the FFT
  FFT.ComplexToMagnitude();   // Determines magnitudes from imaginary data (we are not interested in the imaginary part for this FFT application)

  // Determines the dominant frequency and its magnitude in the vReal array
  for (int i = 0; i < 63; i++) {
    if (Vmax < vReal[i]) {
      Vmax = vReal[i];
      Fmax = i;
    }
  }
  //displays the dominant frequency and its magnitude on the serial monitor
  Serial.print("Strongest Frequency: ");
  Serial.print(Fmax);
  Serial.print("Hz at ");
  Serial.print(Vmax);
  Serial.println(" Magnitude Units");

  // Prints new vReal to the serial monitor
  Serial.println("Fourier Transform ");
  for (int i = 0; i < sampleRate; i++) {
    Serial.print(vReal[i]);
    Serial.print(", ");
  }
  Serial.println(" ");

  // Clears the display to remove the old bargraph
  display.clearDisplay();

  // Diplays Column Labels for the bargraph
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

  // Analysis of the FFT, the numbers represent the brainwave frequency ranges
  // Computes the power of delta waves and stores it in the bargraphArray
  bargraphArray[0] = round(averagePower(vReal, 1, 4));

  // Computes the power of theta waves and stores it in the bargraphArray
  bargraphArray[1] = round(averagePower(vReal, 4, 7));

  // Computes the power of alpha waves and stores it in the bargraphArray
  bargraphArray[2] = round(averagePower(vReal, 7, 11));

  // Computes the power of beta waves and stores it in the bargraphArray
  bargraphArray[3] = round(averagePower(vReal, 11, 27));

  // Computes the power of gamma waves and stores it in the bargraphArray
  bargraphArray[4] = round(averagePower(vReal, 27, 46));


  //Determines the strongest brain to find a division factor to scale the graph to the small OLED screen
  for (int i = 0; i < 4; i++) {
    if (Bmax < bargraphArray[i]) {
      Bmax = bargraphArray[i];
    }
  }

  // Calculates the division factor to scale the bar graph to the small OLED screen
  divider = round(Bmax / 56);

  // Displays the data scaled brain wave data on the serial monitor
  Serial.print("Brain Waves: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(round(bargraphArray[i] / divider));
    Serial.print(", ");
  }
  Serial.println(" ");

  // Displays the scaled brain wave data on the bar graph
  display.fillRect(0, (64 - (round(bargraphArray[0] / divider))), 24, round(bargraphArray[0] / divider), WHITE);
  display.fillRect(26, (64 - (round(bargraphArray[1] / divider))), 24, round(bargraphArray[1] / divider), WHITE);
  display.fillRect(52, (64 - (round(bargraphArray[2] / divider))), 24, round(bargraphArray[2] / divider), WHITE);
  display.fillRect(78, (64 - (round(bargraphArray[3] / divider))), 24, round(bargraphArray[3] / divider), WHITE);
  display.fillRect(104, (64 - (round(bargraphArray[4] / divider))), 24, round(bargraphArray[4] / divider), WHITE);
  display.display();

  // Clears the bargraphArray
  for (int i = 0; i < 4; i++) {
    bargraphArray[i] = 0;
  }

  // Resets the variables used to determine the dominant brain wave frequency and bargraph height divider
  Vmax = 0;
  Fmax = 0;
  Bmax = 0;

  //  delay(10000);
}

// Average power function that is used to calculate the average RMS power of each of the brain wave frequency ranges
double averagePower(double realArray[], int start, int finish) {
  double sum;
  int coefficientCount = finish - start;
  for (start; start < finish; start++) {
    sum = (sq(realArray[start]));
  }
  return sqrt(sum / coefficientCount);
}
