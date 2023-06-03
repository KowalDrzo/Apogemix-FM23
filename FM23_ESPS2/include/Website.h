#ifndef WEBSITE_H
#define WEBSITE_H

#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>

#include "Tasks.h"

class Website {

    enum SettingsType {
        RECOVERY_SETTINGS = 0,
        TELEMETRY_SETTINGS,
        SERVOS_SETTINGS
    };

    String ssid = String("Apogemix_FM23_") + String(APOGEMIX_ID);
    const char* password = "ApogemixFM23";
    AsyncWebServer server = AsyncWebServer(80);
    bool enabled = false;

    String generateHtml();

public:

    bool isEnabled() { return enabled; }
    void start();
    void stop();
};

#endif