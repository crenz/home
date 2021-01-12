#include "config.h"
#include "credentials.h"

void setup() {
    msgSetup();
    msgln("Main", "--------------------");
    networkSetup();
    sensorsSetup();
    epdSetup();
}

void loop() {
    msgLoop();
    networkLoop();
    sensorsLoop();
    epdLoop();
}
