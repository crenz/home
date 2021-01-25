ADC_MODE(ADC_VCC); 

long lastPublishRateMS = 0;

float sensorsGetVcc () {
    return ESP.getVcc() / 1000.0;
}

long sensorsGetRSSI () {
    return WiFi.RSSI();
}

void sensorsSend() {
    StaticJsonDocument<1024> doc;
    String output;

    doc["sensor"] = idString;

    doc["esp8266Vcc"] = sensorsGetVcc();
    doc["esp8266Uptime"] = millis();
    doc["esp8266WifiRssi"] = sensorsGetRSSI();

    doc["bme680RawTemperature"] = iaqSensor.rawTemperature;
    doc["bme680pressure"] = iaqSensor.pressure;
    doc["bme680RawHumidity"] = iaqSensor.rawHumidity;
    doc["bme680GasResistance"] = iaqSensor.gasResistance;
    doc["bme680Iaq"] = iaqSensor.iaq;
    doc["bme680IaqAccuracy"] = iaqSensor.iaqAccuracy;
    doc["bme680Temperature"] = iaqSensor.temperature;
    doc["bme680Humidity"] = iaqSensor.humidity;
    doc["bme680StaticIaq"] = iaqSensor.staticIaq;
    doc["bme680Co2Equivalent"] = iaqSensor.co2Equivalent;
    doc["bme680BreathVocEquivalent"] = iaqSensor.breathVocEquivalent;

    serializeJson(doc, output);

    networkMqttPublish(configMqttTopic, output);
}


void sensorsSetup() {
    
}

void sensorsLoop() {
    if (millis() - lastPublishRateMS > configPublishRateMS) {
        lastPublishRateMS = millis();
        sensorsSend();
    }
}
