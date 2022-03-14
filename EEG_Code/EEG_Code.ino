#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// On an arduino UNO:       A4(SDA), A5(SCL)

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);


// This will allow us to use the byte data type instead of floats when calculating sin and cos in the FFT

byte sine_data [91] = {
  0,
  4,    9,    13,   18,   22,   27,   31,   35,   40,   44,
  49,   53,   57,   62,   66,   70,   75,   79,   83,   87,
  91,   96,   100,  104,  108,  112,  116,  120,  124,  127,
  131,  135,  139,  143,  146,  150,  153,  157,  160,  164,
  167,  171,  174,  177,  180,  183,  186,  189,  192,  195,
  198,  201,  204,  206,  209,  211,  214,  216,  219,  221,
  223,  225,  227,  229,  231,  233,  235,  236,  238,  240,
  241,  243,  244,  245,  246,  247,  248,  249,  250,  251,
  252,  253,  253,  254,  254,  254,  255,  255,  255,  255
};

float f_peaks[5]; // 5 dominant frequencies in descending order

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);
}

void loop() {
  const int sampleRate = 128;
  int sample = 0;

  int dataArray[sampleRate];

  while (sample <= sampleRate) {
    double signalVoltage = analogRead(1) / 204.8;
    dataArray[sample] = signalVoltage;
    delay(round(1024 / sampleRate));
    sample++;
  }

  for (int i = 0; i < sampleRate; i++) {
    Serial.println(dataArray[i]);
  }

  // Calling The FFT Function
  FFT(dataArray, 128, 128);
  Serial.println(f_peaks[0]);
  Serial.println(f_peaks[1]);
  delay(99999);

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

  Serial.println(signalVoltage);

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

float FFT(int in[], int N, float Frequency)
{
  unsigned int data[13] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  int a, c1, f, o, x;
  a = N;

  for (int i = 0; i < 12; i++)          //calculating the levels
  {
    if (data[i] <= a) {
      o = i;
    }
  }

  int in_ps[data[o]] = {};              //input for sequencing
  float out_r[data[o]] = {};            //real part of transform
  float out_im[data[o]] = {};           //imaginory part of transform

  x = 0;
  for (int b = 0; b < o; b++)           // bit reversal
  {
    c1 = data[b];
    f = data[o] / (c1 + c1);
    for (int j = 0; j < c1; j++)
    {
      x = x + 1;
      in_ps[x] = in_ps[j] + f;
    }
  }

  for (int i = 0; i < data[o]; i++)     // update input array as per bit reverse order
  {
    if (in_ps[i] < a)
    {
      out_r[i] = in[in_ps[i]];
    }
    if (in_ps[i] > a)
    {
      out_r[i] = in[in_ps[i] - a];
    }
  }

  int i10, i11, n1;
  float e, c, s, tr, ti;

  for (int i = 0; i < o; i++)          //fft
  {
    i10 = data[i];                     // overall values of sine/cosine  :
    i11 = data[o] / data[i + 1];       // loop with similar sine cosine:
    e = 360 / data[i + 1];
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++)
    {
      c = cosine(e * j);
      s = sine(e * j);
      n1 = j;

      for (int k = 0; k < i11; k++)
      {
        tr = c * out_r[i10 + n1] - s * out_im[i10 + n1];
        ti = s * out_r[i10 + n1] + c * out_im[i10 + n1];

        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;

        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;

        n1 = n1 + i10 + i10;
      }
    }
  }

  // Here forward, out_r contains amplitude and our_in conntains frequency (Hz)
  for (int i = 0; i < data[o - 1]; i++)                             // getting amplitude from compex number
  {
    out_r[i] = sqrt(out_r[i] * out_r[i] + out_im[i] * out_im[i]);   // to  increase the speed delete sqrt
    out_im[i] = i * Frequency / N;
  }

  x = 0;                                                            // peak detection
  for (int i = 1; i < data[o - 1] - 1; i++)
  {
    if (out_r[i] > out_r[i - 1] && out_r[i] > out_r[i + 1])
    { in_ps[x] = i;                                                 // in_ps array used for storage of peak number
      x = x + 1;
    }
  }


  s = 0;
  c = 0;
  for (int i = 0; i < x; i++)                                       // re-arraange as per magnitude
  {
    for (int j = c; j < x; j++)
    {
      if (out_r[in_ps[i]] < out_r[in_ps[j]])
      { s = in_ps[i];
        in_ps[i] = in_ps[j];
        in_ps[j] = s;
      }
    }
    c = c + 1;
  }


  for (int i = 0; i < 5; i++)                                       // updating f_peak array (global variable)with descending order
  {
    f_peaks[i] = out_im[in_ps[i]];
  }
}


float sine(int i)
{
  int j = i;
  float out;
  while (j < 0) {
    j = j + 360;
  }
  while (j > 360) {
    j = j - 360;
  }
  if (j > -1   && j < 91) {
    out = sine_data[j];
  }
  else if (j > 90  && j < 181) {
    out = sine_data[180 - j];
  }
  else if (j > 180 && j < 271) {
    out = -sine_data[j - 180];
  }
  else if (j > 270 && j < 361) {
    out = -sine_data[360 - j];
  }
  return (out / 255);
}

float cosine(int i)
{
  int j = i;
  float out;
  while (j < 0) {
    j = j + 360;
  }
  while (j > 360) {
    j = j - 360;
  }
  if (j > -1   && j < 91) {
    out = sine_data[90 - j];
  }
  else if (j > 90  && j < 181) {
    out = -sine_data[j - 90];
  }
  else if (j > 180 && j < 271) {
    out = -sine_data[270 - j];
  }
  else if (j > 270 && j < 361) {
    out = sine_data[j - 270];
  }
  return (out / 255);
}
