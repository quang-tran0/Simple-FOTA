#include "coreiot_task.h"

WiFiClient espClient;
PubSubClient client(espClient);

void coreiotSetup() {
    client.setServer(SERVER_URL, 1883);
    client.setCallback(callback);
    coreiotConnect();

    client.subscribe("v1/devices/me/attributes/response/+");
    client.subscribe("v1/devices/me/attributes");
    client.publish("v1/devices/me/attributes/request/10", "{\"sharedKeys\":\"fw_version,fw_title\"}");
}

void extractFirmwareInfo(JsonObject doc) {
    if (doc.containsKey("fw_title") && doc.containsKey("fw_version")) {
        String newTitle = doc["fw_title"].as<String>();
        String newVersion = doc["fw_version"].as<String>();

        Serial.println("Received firmware update info - Title: " + newTitle + ", Version: " + newVersion);

        if (newTitle != currentFirmwareTitle || newVersion != currentFirmwareVersion) {
            Serial.println("New firmware available. Downloading...");
            currentFirmwareTitle = newTitle;
            currentFirmwareVersion = newVersion;
            pendingFirmwareDownload = true;
        } else {
            Serial.println("Firmware is up to date.");
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    String responeAttributeTopic = "v1/devices/me/attributes/response/10";
    String attributeUpdateTopic = "v1/devices/me/attributes";

    // Shared attribute response
    if (String(topic).equals(responeAttributeTopic)) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
            Serial.print("Failed to parse JSON: ");
            Serial.println(error.c_str());
            return;
        }
        
        extractFirmwareInfo(doc["shared"]);

    // Shared attribute update
    } else if (String(topic).equals(attributeUpdateTopic)) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
            Serial.print("Failed to parse JSON: ");
            Serial.println(error.c_str());
            return;
        }
        
        extractFirmwareInfo(doc.as<JsonObject>());
    }
}

void coreiotConnect() {
    while (1) {
        if (client.connect("ESP32_Gateway_Simple-FOTA", DEVICE_TOKEN, NULL)) {
            Serial.println("Connected to CoreIoT Server");
            break;
        } else {
            Serial.print("Failed to connect to CoreIoT Server, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void coreiotTask(void *pvParameters) {
    coreiotSetup();

    while (1) {
        if (!client.connected()) {
            Serial.println("Disconnected from CoreIoT Server. Attempting to reconnect...");
            coreiotConnect();
        }

        client.loop();

        vTaskDelay(2000);
    }
}