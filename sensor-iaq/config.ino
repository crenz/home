/* Example JSON.

{
  "logSerial": true,
  "logVerbose": true,
  "ledBrightness": 5,
  "temperatureOffset": 4.1,
  "publishRateMS": 10000,
  "mqttTopic": "sensor/garage"
}
 
 
*/


void configUpdate(const String& payload) {
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        msgln("Config", "Received config message, deserialization failed: " + String(error.c_str()));
        return;
    }

    msgln("Config", "Received config message, updating configuration.");

    if (doc.containsKey("logSerial")) {
        configLogSerial = doc["logSerial"];
    }
    if (doc.containsKey("logVerbose")) {
        configLogVerbose = doc["logVerbose"];
    }
    if (doc.containsKey("ledBrightness")) {
        configLedBrightness = doc["ledBrightness"];
    }
    if (doc.containsKey("temperatureOffset")) {
        configTemperatureOffset = doc["temperatureOffset"];
        bsecUpdateTemperatureOffset(configTemperatureOffset);
    }
    if (doc.containsKey("publishRateMS")) {
        configPublishRateMS = doc["publishRateMS"];
    }
    if (doc.containsKey("mqttTopic")) {
        configMqttTopic = doc["mqttTopic"].as<String>();
    }
}
