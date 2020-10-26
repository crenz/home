
void configUpdate(const String& payload) {
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        msgln("Config", "Received config message, deserialization failed: " + String(error.c_str()));
        return;
    }

    msgln("Config", "Received config message, updating configuration.");

    
    configLogSerial = doc["logSerial"];
    configLogVerbose = doc["logVerbose"];
    configLedBrightness = doc["ledBrightness"];
    configTemperatureOffset = doc["temperatureOffset"];
    bsecUpdateTemperatureOffset(configTemperatureOffset);
}
