#include "config.h"
#include "credentials.h"

void setup() {
    msgSetup();
    msgln("Main", "--------------------");
    msgln("sensor-iaq v1.0");
    bsecSetup();
    networkSetup();
    sensorsSetup();
}

void loop() {
    bsecLoop();
    msgLoop();
    networkLoop();
    sensorsLoop();
}
