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

void networkMqttMessageReceived(MQTTClient *client, char topic[], char payload[], int payload_length);

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
            mqtt.onMessageAdvanced(networkMqttMessageReceived);
            
            mqtt.subscribe("/config/" + String(idString));
            mqtt.subscribe("/display/" + String(idString) + "/#");
/*            
            if (!mqtt.subscribe("/config/" + String(idString))) {
                msgln("MQTT", "Failed to subscribe to config messages");
            }
            if (!mqtt.subscribe("/display/" + String(idString) + "/#")) {
                msgln("MQTT", "Failed to subscribe to display messages");              
            }
*/
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

void networkMqttMessageReceived(MQTTClient *client, char topic[], char payload[], int payload_length) {
    msgln("MQTT", "Received MQTT message on topic '" + String(topic) + "', len=" + String(payload_length));

    if (strstr(topic, "/config") != NULL) {
        msgln("MQTT", "Updating configuration");
        configUpdate(payload);
    } else if (strstr(topic, "/display") != NULL) {
        msgln("MQTT", "Updating display");
    } else {
        epdProcessMessage(topic, payload, payload_length);
        msgln("MQTT", "Unknown message");
    }
}

void networkMqttPublish(String topic, String payload) {
    msgln("MQTT", "Publishing MQTT message on topic '" + topic + "'");

    mqtt.publish(topic, payload);
}

void networkSetup() {
    byte macAddress[6];
    WiFi.macAddress(macAddress);
    sprintf(idString, "epd-%02x%02x%02x",
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
