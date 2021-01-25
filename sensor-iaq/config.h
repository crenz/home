#include <ArduinoJson.h>

bool configLogSerial = true;
bool configLogVerbose = true;

char idString[11] = "iaq-xxxxxx";

int configLedBrightness = 5;

int configTemperatureOffset = 4.1f;

int configPublishRateMS = 10000; // Upload sensor values every x ms

String configMqttTopic = "sensor/iaq-xxxxxx";
