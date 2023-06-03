#ifndef LOOPS_H
#define LOOPS_H

#include "Website.h"
#include <LoRa.h>

class StateLoops {

    Timer pressMeasureTimer;
    Timer waitingTimer;
    Timer wifiTimer;

    String rxDebugString;

    void dataLoop(bool enableFlashWrite);
    void ignitionLoop(bool apogee);
    void waitAndLogData(uint32_t time_ms);

public:

    void railLoop();
    void flightLoop();
    void sepLoop();
    void groundLoop();
};

extern StateLoops loops;

#endif