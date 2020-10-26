#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <MQTT.h>

enum TNetworkStatus {
  NS_DISCONNECTED,
  NS_CONNECTING,
  NS_CONNECTED
};

TNetworkStatus networkWiFiStatus = NS_DISCONNECTED;
TNetworkStatus networkMqttStatus = NS_DISCONNECTED;

WiFiClient wifi;
MQTTClient mqtt(1024);
char mqttClientId[100];

void networkConnectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        if (networkWiFiStatus != NS_CONNECTED) {
            msg("WiFi", "Connection established, IP address: ");
            msgln(WiFi.localIP());
            networkWiFiStatus = NS_CONNECTED;
            MDNS.begin(idString);
        }
        return;
    }

    // If connect already in progress, nothing to be done
    if (networkWiFiStatus == NS_CONNECTING) return;

    if (networkWiFiStatus == NS_CONNECTED) {
        msgln("WiFi", "Detected WiFi disconnect, reconnecting to " + String(wifiSSID));
    } else {
        msgln("WiFi", "Connecting to WiFi network " + String(wifiSSID));      
    }

    // Connect necessary
    networkWiFiStatus = NS_CONNECTING;
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);
}

void networkConnectMqtt() {
    if (networkWiFiStatus != NS_CONNECTED) return;
  
    if (mqtt.connected()) {
        if (networkMqttStatus != NS_CONNECTED) {
            msgln("MQTT", "Connection established");
            networkMqttStatus = NS_CONNECTED;
            mqtt.onMessage(networkMqttMessageReceived);
            mqtt.subscribe("/config/" + String(idString));
        }
        return;
    }

    // If connect already in progress, nothing to be done
    if (networkMqttStatus == NS_CONNECTING) {
        mqtt.connect(mqttClientId, mqttUser, mqttPassword);
        return;
    }

    if (networkMqttStatus == NS_CONNECTED) {
        msgln("MQTT", "Detected disconnect, reconnecting to " + String(mqttBroker));
    } else {
        msgln("MQTT", "Connecting to broker " + String(mqttBroker));      
    }

    // Connect necessary
    networkMqttStatus = NS_CONNECTING;
    mqtt.connect(mqttClientId, mqttUser, mqttPassword);
}

void networkMqttMessageReceived(String &topic, String &payload) {
    msgln("MQTT", "Received MQTT message on topic '" + topic + "'");

    // For now, only subscribing to config updates -> pass it on to config module
    configUpdate(payload);
}

void networkMqttPublish(String topic, String payload) {
    msgln("MQTT", "Publishing MQTT message on topic '" + topic + "'");

    mqtt.publish(topic, payload);
}

void networkSetup() {
    byte macAddress[6];
    WiFi.macAddress(macAddress);
    sprintf(idString, "iaq-%02x%02x%02x",
        macAddress[3],
        macAddress[4],
        macAddress[5]);

    msgln("Network", "ID/host name: " + String(idString));

    sprintf(mqttClientId, "%s-%06x", idString, random(256*256*256));
    mqtt.begin(mqttBroker, wifi);
    msgln("MQTT", "Cient ID: " + String(mqttClientId));
  
    networkConnectWiFi();
    networkConnectMqtt();
}

void networkLoop() {
    networkConnectWiFi();
    networkConnectMqtt();
    delay(10);
    mqtt.loop();    
}
