#include "Website.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void Website::start() {

    WiFi.softAP(ssid.c_str(), password);
    MDNS.begin("apogemix");

    Serial.println("server on");
    enabled = true;

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {

        request->send(200, "text/html", generateHtml());
    });

    server.on("/api", HTTP_GET, [this](AsyncWebServerRequest *request) {

        request->send(200, "text/html", glob.dataFrame.toString());
    });

    server.on("/wifioff", HTTP_GET, [this](AsyncWebServerRequest *request) {

        request->send(200, "text/html", String("<meta http-equiv=\"refresh\" content=\"0; URL=/\" />"));
        stop();
    });

    server.on("/FlightData.apg", HTTP_GET, [this](AsyncWebServerRequest *request) {

        request->send(LITTLEFS, "/FlightData.apg", String());
    });

    server.begin();
}

#pragma GCC diagnostic pop

/*********************************************************************/

void Website::stop() {

    Serial.println("server off");
    WiFi.softAPdisconnect(true);
    enabled = false;
}

/*********************************************************************/

String Website::generateHtml() {

    String html = R"rawliteral(
        <!DOCTYPE html>
        <head>
            <title>Apogemix FM23 configuration page</title>
        </head>
        <body>
            <h3>Apogemix FM23</h3><hr>
            </p><hr>
            <a href='/FlightData.apg'>Download data from last flight.</a>
            <p>Flight data table:
            <table border='1'>
                <tr>
                    <th>Num</th><th>Apogee [m]</th><th>Max speed [m/s]</th>
                </tr>
    )rawliteral";

    for (uint8_t i = 0; i < FLIGHTS_IN_MEM; i++) {

        if (glob.memory.flight[i].apogee || glob.memory.flight[i].maxSpeed) {
            html += "<tr><td>" + String(glob.memory.flight[i].num) + "</td>";
            html += "<td>" + String(glob.memory.flight[i].apogee) + "</td>";
            html += "<td>" + String(glob.memory.flight[i].maxSpeed) + "</td></tr>\n";
        }
    }

    html += R"rawliteral(
            </table>
            </p><hr>
            <a href='/wifioff'>Turn off the wifi.</a>
        </body>
        </html>
    )rawliteral";

    return html;
}
