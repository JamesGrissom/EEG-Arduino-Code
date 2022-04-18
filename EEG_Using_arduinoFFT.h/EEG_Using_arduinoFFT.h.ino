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
int maxV = 0;
arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, sampleRate);

// On an Arduino:      A4(SDA), A5(SCL)
// On esp32 devkitc v4 21(SDA), 22(SCL)

// Sensor pins:
int ArduinoSensor = 1;
int esp32Sensor = 26;

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Stores the scaled brainwave power to be displayed in a bar graph
int waveGraph;

// averagePower function Declarations
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
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();
  int Vmax = 0;
  int Fmax = 0;
  for (int i = 0; i < 63; i++) {
    if (Vmax < vReal[i]) {
      Vmax = vReal[i];
      Fmax = i;
    }
  }
  Serial.print("Strongest Frequency: ");
  Serial.print(Fmax);
  Serial.print("Hz at ");
  Serial.print(Vmax);
  Serial.println(" Volts");

  // Prints new vReal to the serial monitor
  Serial.println("Fourier Transform ");
  for (int i = 0; i < sampleRate; i++) {
    Serial.print(vReal[i]);
    Serial.print(", ");
  }
  Serial.println(" ");

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
    if (maxV < bargraphArray[i]) {
      maxV = bargraphArray[i];
    }
  }

  // Calculates the division factor to scale the bar graph to the small OLED screen
  divider = round(maxV / 56);

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

  int maxV = 0;

//  delay(10000);
}

double averagePower(double realArray[], int start, int finish) {
  double sum;
  int coefficientCount = finish - start;
  for (start; start < finish; start++) {
    sum = (sq(realArray[start]));
  }
  return sqrt(sum / coefficientCount);
}
