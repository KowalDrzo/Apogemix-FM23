#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <LITTLEFS.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>
#include <TinyGPS++.h>
#include <Servo.h>

#include "PinDefinitions.h"
#include "Timer.h"
#include "DataStructs.h"
#include "Globals.h"

class Tasks {

    uint8_t criteriaCounter = 0;
    void continuityTest();
    Servo servos[2];
    bool serialOpened = false;
    float getPressureMedian();

public:

    Adafruit_BMP085 bmp;
    TinyGPSPlus gps;

    void buzzBeep(uint16_t activeTime, uint16_t sleepTime, uint8_t n);

    void measure();
    void buzz();

    bool isLaunchDetected();
    bool isApogeeDetected();
    bool isOnGround();

    static void flashTask();
    void readFlash();
    void updateDataBase();
    void recalibrate();
    void checkSerial(uint8_t blockTime_s);

    void clearMem();
};

extern Tasks tasks;

#endif