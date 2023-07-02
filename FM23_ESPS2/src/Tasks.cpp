#include "Tasks.h"

Tasks tasks;

#define CRITERIA_MARGIN 5

void Tasks::continuityTest() {

    glob.dataFrame.continuity = !digitalRead(CONT_PIN);
}

/*********************************************************************/

void Tasks::measure() {

    // Pressure and temperature:
    glob.dataFrame.pressure = glob.bmpFilter.getPress();
    glob.dataFrame.temper = glob.bmpFilter.getTemp();

    // Time:
    uint32_t newTime = millis();
    uint32_t timeDiff = newTime - glob.dataFrame.time;
    if (!timeDiff) timeDiff++;
    glob.dataFrame.time = newTime;

    // Filtered altitude and speed:
    glob.dataFrame.altitude = glob.bmpFilter.getAlt();
    glob.dataFrame.speed = glob.bmpFilter.getSpeed();

    // Contunuity:
    continuityTest();

    // Maximas:
    if (glob.apogee < glob.dataFrame.altitude) glob.apogee = glob.dataFrame.altitude;
    if (glob.maxSpeed < glob.dataFrame.speed) glob.maxSpeed = glob.dataFrame.speed;

    // Debug:
    Serial.println(glob.dataFrame.toString());
}

/*********************************************************************/

void Tasks::buzzBeep(uint16_t activeTime, uint16_t sleepTime, uint8_t n) {

    for (; n > 0; n--) {

        digitalWrite(BUZZER_PIN, 1);
        vTaskDelay(activeTime / portTICK_PERIOD_MS);
        digitalWrite(BUZZER_PIN, 0);
        vTaskDelay(sleepTime / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/

void Tasks::buzz() {

    continuityTest();

    buzzBeep(30, 150, 2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    if (glob.dataFrame.continuity)                            buzzBeep(500, 500, 1);
}

/*********************************************************************/

bool Tasks::isLaunchDetected() {

    if (glob.dataFrame.altitude > 20 && glob.dataFrame.speed > 1) {

        criteriaCounter++;
        if (criteriaCounter >= CRITERIA_MARGIN) {

            criteriaCounter = 0;
            return true;
        }
    }
    else criteriaCounter = 0;

    return false;
}

/*********************************************************************/

bool Tasks::isApogeeDetected() {

    if (glob.dataFrame.speed < 10) {

        criteriaCounter++;
        if (criteriaCounter >= CRITERIA_MARGIN) {

            criteriaCounter = 0;
            return true;
        }
    }
    else criteriaCounter = 0;

    return false;
}

/*********************************************************************/

bool Tasks::isOnGround() {

    if (abs(glob.dataFrame.speed) < 2) {

        criteriaCounter++;
        if (criteriaCounter >= CRITERIA_MARGIN) {

            criteriaCounter = 0;
            return true;
        }
    }
    else criteriaCounter = 0;

    return false;
}

/*********************************************************************/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void Tasks::flashTask() {

    File file;
    bool appendFlash = false;

    while (glob.dataFrame.rocketState < FLIGHT) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    while (1) {

        if (uxQueueMessagesWaiting(glob.dataFramesFifo) > 20 || glob.dataFrame.rocketState == GROUND) {

            if (!appendFlash) {
                appendFlash = true;
                file = LITTLEFS.open("/FlightData.apg", "w");
            }
            else {
                file = LITTLEFS.open("/FlightData.apg", "a");
            }

            while (uxQueueMessagesWaiting(glob.dataFramesFifo)) {

                DataFrame tempData;
                xQueueReceive(glob.dataFramesFifo, &tempData, portMAX_DELAY);

                char tempDataAscii[120];
                strcpy(tempDataAscii, tempData.toString().c_str());
                strcat(tempDataAscii, "\n");
                file.write((uint8_t*) tempDataAscii, strlen(tempDataAscii));
            }

            file.close();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/

void Tasks::readFlash() {

    LITTLEFS.begin(true);

    File file = LITTLEFS.open("/FlightData.apg", "r");

    if (serialOpened) {
        while (file.available()) Serial.print(file.readString());
    }

    file.close();
}

#pragma GCC diagnostic pop

/*********************************************************************/

void Tasks::filterTask() {

    vTaskDelay(500 / portTICK_PERIOD_MS);

    while(1) {

        glob.bmpFilter.feedFilter(millis(), glob.initialPressure, glob.initialTemper);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/

void Tasks::clearMem() {

    LITTLEFS.begin(true);
    File file = LITTLEFS.open("/FlightData.apg", "w");

    memset(&glob.memory, 0, sizeof(glob.memory));

    EEPROM.put(0, glob.memory);
    EEPROM.commit();

    buzzBeep(30, 30, 10);
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/

void Tasks::updateDataBase() {

    glob.memory.lastFlightIndex++;
    if (glob.memory.lastFlightIndex > FLIGHTS_IN_MEM) glob.memory.lastFlightIndex = 0;
    uint8_t flid = glob.memory.lastFlightIndex;

    glob.memory.lastFlightNum++;

    glob.memory.flight[flid].num = glob.memory.lastFlightNum;
    glob.memory.flight[flid].apogee = glob.apogee;
    glob.memory.flight[flid].maxSpeed = glob.maxSpeed;

    EEPROM.put(0, glob.memory);
    EEPROM.commit();
}

/*********************************************************************/

void Tasks::recalibrate() {

    glob.initialPressure = glob.bmpFilter.getPress();
    glob.initialTemper = glob.bmpFilter.getTemp();
}

/*********************************************************************/

void Tasks::checkSerial(uint8_t blockTime_s) {

    for (uint8_t i = 0; i < blockTime_s; i++) {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (Serial) {
            serialOpened = true;
            return;
        }
    }
}