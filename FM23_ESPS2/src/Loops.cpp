#include "Loops.h"

StateLoops loops;

void StateLoops::dataLoop(bool enableFlashWrite) {

    tasks.measure();
    DataFrame trashBin;
    if (uxQueueMessagesWaiting(glob.dataFramesFifo) == FRAMES_IN_Q) {
        xQueueReceive(glob.dataFramesFifo, &trashBin, portMAX_DELAY);
    }
    xQueueSend(glob.dataFramesFifo, &glob.dataFrame, portMAX_DELAY);
}

/*********************************************************************/

void StateLoops::ignitionLoop(bool apogee) {

    digitalWrite(SEPAR_PIN, 1);
    waitAndLogData(FIRE_TIME);
    digitalWrite(SEPAR_PIN, 0);
}

/*********************************************************************/

void StateLoops::waitAndLogData(uint32_t time_ms) {

    waitingTimer.start(time_ms);

    while (!waitingTimer.check()) {

        if (pressMeasureTimer.check()) dataLoop(1);

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/
/*********************************************************************/

void StateLoops::railLoop() {

    pressMeasureTimer.start(RAIL_FLIGHT_LOOP_TIME);
    Website website;

    while (1) {

        if (pressMeasureTimer.check()) {

            dataLoop(0);
            if (tasks.isLaunchDetected()) {

                if (website.isEnabled()) website.stop();
                break;
            }

            // WiFi itp:
            if (!digitalRead(SWITCH_PIN) && !website.isEnabled()) {

                website.start();
                wifiTimer.start(WIFI_TIME_MS);
            }
            if (website.isEnabled() && wifiTimer.check()) {

                website.stop();
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);

        // DEBUG:
        if (Serial.available()) {
            rxDebugString = Serial.readString();
            if (strstr(rxDebugString.c_str(), "FORCE NEXT STATE")) {

                if (website.isEnabled()) website.stop();
                break;
            }
        }
    }
}

/*********************************************************************/

void StateLoops::flightLoop() {

    pressMeasureTimer.start(RAIL_FLIGHT_LOOP_TIME);

    while(1) {

        if (pressMeasureTimer.check()) {

            dataLoop(1);
            if (tasks.isApogeeDetected()) {

                ignitionLoop(1);
                break;
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);

        // DEBUG:
        if (Serial.available()) {
            rxDebugString = Serial.readString();
            if (strstr(rxDebugString.c_str(), "FORCE NEXT STATE")) break;
        }
    }
}

/*********************************************************************/

void StateLoops::sepLoop() {

    pressMeasureTimer.start(FIRST_SEPAR_LOOP_TIME);

    while(1) {

        if (pressMeasureTimer.check()) {

            dataLoop(1);
            if (tasks.isOnGround()) {

                ignitionLoop(0);
                break;
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************/

void StateLoops::groundLoop() {

    vTaskDelay(3000 / portTICK_PERIOD_MS);
}
