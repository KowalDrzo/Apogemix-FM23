#ifndef BMP_FILTER_H
#define BMP_FILTER_H

#include <Wire.h>
#include <Adafruit_BMP085.h>

#include "DataStructs.h"

class BmpFilter {

    Adafruit_BMP085 bmp;

    uint32_t lastTime_ms = 0;
    float forecastAlt = 0;
    float forecastSpeed = 0;

public:

    bool begin(TwoWire *wire) { return bmp.begin(BMP085_ULTRAHIGHRES, wire); }
    float getTemp() { return bmp.readTemperature() * TEMPERATURE_FIX_A + TEMPERATURE_FIX_B; }
    float getPress() { return bmp.readPressure(); }

    float getAlt() { return forecastAlt; }
    float getSpeed() { return forecastSpeed; }

    void feedFilter(uint32_t time_ms, float initialPress, float initialTemp);
};

#endif