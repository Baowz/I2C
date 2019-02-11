#include <Arduino.h>

#define PMsensorErrSuccess 100
#define PMsensorErrDataLow 101
#define PMsensorErrDataZero 102

class PMsensor {
  public:
    PMsensor();
  void init(int LEDpin, int sensorPin);
    int read(float* pdata, bool isfilter);
    int read(float* pdata, bool isfilter, float sensitivity);
    float rawData;
    float val;
    float prevVal;

  private:
    int _LEDpin;
    int _sensorPin;
    float _sensitivity;

};
