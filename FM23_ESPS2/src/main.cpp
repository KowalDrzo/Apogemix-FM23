#include "Loops.h"

GlobalAggregator glob;

void setup() {

    Serial.begin(115200);
    Serial.setTimeout(10);
    tasks.checkSerial(5);

    Wire.begin(SDA_PIN, SCL_PIN, 125000);
    tasks.bmp.begin(BMP085_HIGHRES, &Wire);

    pinMode(SEPAR_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(CONT_PIN, INPUT_PULLUP);
    pinMode(SWITCH_PIN, INPUT_PULLUP);

    // CONFIG:
    EEPROM.begin(sizeof(glob.memory));
    EEPROM.get(0, glob.memory);

    // Only for first programming:
    //tasks.clearMem();

    tasks.readFlash();
    tasks.recalibrate();
    Serial.println(glob.initialTemper);
    tasks.buzz();

    // RTOS queues:
    glob.dataFramesFifo = xQueueCreate(FRAMES_IN_Q, sizeof(DataFrame));

    // Pararell tasks:
    xTaskCreate((TaskFunction_t) Tasks::flashTask, "Flash Task", 16384, NULL, 1, NULL);

    // LOOPS:
    glob.dataFrame.rocketState = RAIL;
    Serial.println("RAIL STATE");
    loops.railLoop();

    glob.dataFrame.rocketState = FLIGHT;
    Serial.println("FLIGHT STATE");
    loops.flightLoop();

    glob.dataFrame.rocketState = SEPAR;
    tasks.updateDataBase();
    Serial.println("SEPAR STATE");
    loops.sepLoop();

    glob.dataFrame.rocketState = GROUND;
    Serial.println("GROUND STATE");
    loops.groundLoop();
}

void loop() {

    digitalWrite(BUZZER_PIN, 1);
    Serial.println("Waiting for pickup");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(BUZZER_PIN, 0);
    vTaskDelay(9000 / portTICK_PERIOD_MS);
}