#include <ArduinoJson.h>

bool configLogSerial = true;
bool configLogVerbose = true;

char idString[11] = "epd-xxxxxx";

int configLedBrightness = 5;

int configPublishRateMS = 10000; // Upload sensor values every x ms
