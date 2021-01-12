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

    serializeJson(doc, output);

    networkMqttPublish("/sensors/" + String(idString), output);
}


void sensorsSetup() {
    
}

void sensorsLoop() {
    if (millis() - lastPublishRateMS > configPublishRateMS) {
        lastPublishRateMS = millis();
        sensorsSend();
    }
}
