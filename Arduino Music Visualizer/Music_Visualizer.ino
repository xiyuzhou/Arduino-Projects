#include <U8g2lib.h>
#include <arduinoFFT.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

#define samples 32
#define halfsamples samples / 2
#define NumofCopy halfsamples * sizeof(double)
#define Interval 128 / (halfsamples)

arduinoFFT FFT = arduinoFFT();

double vReal[samples];
double vImag[samples];
double vTemp[halfsamples];
//const int multiplier[halfsamples] = {3,6,20,21,22,23,24,25,26,27,28,29,30,31,32,33};
const int multiplier[halfsamples] = {33, 32, 31, 30, 29, 28, 27, 26, 25, 23, 21, 19, 17, 15, 3, 2};
void setup() {
  setP32();  // 设置32分频,加快AD采样
  u8g2.begin();
}

void loop() {
  for (int i = 0; i < samples; i++) {
    vReal[i] = analogRead(A2) - 512;  // 采样
    vImag[i] = 0.0;
  }

  u8g2.clearBuffer();
  
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HANN, FFT_FORWARD); // 加窗
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD);               // 计算fft
  FFT.ComplexToMagnitude(vReal, vImag, samples);                 // 计算幅度

  for (int i = 0; i < halfsamples; i++) {
    double temp = vTemp[halfsamples - i - 1] * multiplier[i] * 0.001;
    double real = vReal[halfsamples - i - 1] * multiplier[i] * 0.001;


    for (int j = 0; j < 6;j++){
      u8g2.drawPixel(i * Interval+j, temp + 1);
      u8g2.drawLine(i * Interval+j, 0, i * Interval+j, real);
    }
  }

  u8g2.sendBuffer();

  memcpy(vTemp, vReal, NumofCopy); // 改为128点时可以注释掉，防止刷新太慢
}

void setP32() // 设置32分频
{
  ADCSRA |= (1 << ADPS2);
  ADCSRA &= ~(1 << ADPS1);
  ADCSRA |= (1 << ADPS0);
}
