#include "BmpFilter.h"

void BmpFilter::feedFilter(uint32_t time_ms, float initialPress, float initialTemp) {

    if (time_ms == lastTime_ms) return;
    float timeDiff = time_ms - lastTime_ms;
    float press = getPress();

    // Altitude:
    float oldForecastAlt = forecastAlt;
    float rawAlt = (initialTemp+273.15)/0.0065*(1.0 - pow(press / initialPress, 0.1903));
    forecastAlt = ALPHA_H * oldForecastAlt + (1-ALPHA_H) * rawAlt;

    // Speed:
    float oldForecastSpeed = forecastSpeed;
    float rawSpeed = 1000.0*(forecastAlt - oldForecastAlt) / timeDiff;
    forecastSpeed = ALPHA_V * oldForecastSpeed + (1-ALPHA_V) * rawSpeed;

    Serial.println(rawAlt);
}
