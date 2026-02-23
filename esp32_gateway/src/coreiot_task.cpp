#include "coreiot_task.h"

WiFiClient espClient;
PubSubClient client(espClient);

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

void coreiotSetup() {
    client.setServer(SERVER_URL, 1883);
    client.setCallback(callback);
    coreiotConnect();

    // Subscribe to RPC requests
    client.subscribe("v1/devices/me/rpc/request/+");

    // Subscribe to attribute updates
    client.subscribe("v1/devices/me/attributes/response/+");
    client.subscribe("v1/devices/me/attributes");
    client.publish("v1/devices/me/attributes/request/10", "{\"sharedKeys\":\"fw_version,fw_title\"}");
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");

    String topicStr(topic);
    String payloadStr;
    for (int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
    Serial.println(payloadStr);

    String attributeTopic = "v1/devices/me/attributes";
    String rpcTopic = "v1/devices/me/rpc/";

    if (topicStr.startsWith(attributeTopic)) 
        handleAttributeTopic(topicStr, payloadStr);
    else if (topicStr.startsWith(rpcTopic))
        handleRpcTopic(topicStr, payloadStr);
}

void handleAttributeTopic(const String& topic, const String& message) {
    String responeAttributeTopic = "v1/devices/me/attributes/response/10";
    String attributeUpdateTopic = "v1/devices/me/attributes";

    // Shared attribute response
    if (topic.equals(responeAttributeTopic)) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
            Serial.print("Failed to parse JSON: ");
            Serial.println(error.c_str());
            return;
        }
        
        extractFirmwareInfo(doc["shared"]);

    // On shared attribute update
    } else if (topic.equals(attributeUpdateTopic)) {
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

void handleRpcTopic(const String& topic, const String& message) {
    if (!topic.startsWith("v1/devices/me/rpc/request/")) return;

    String methodName = extractRpcMethodName(message);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return;

    // Flash firmware RPC Button
    if (methodName.equals("flashFirmware")) {
        pendingFirmwareFlash = true;
        String requestId = topic.substring(String("v1/devices/me/rpc/request/").length());
        String responseTopic = "v1/devices/me/rpc/response/" + requestId;
        client.publish(responseTopic.c_str(), "{\"status\":\"Firmware update initiated\"}");

    // Get auto flash value RPC Switch
    } else if (methodName.equals(("getAutoFlashValue"))) {
        String requestId = topic.substring(String("v1/devices/me/rpc/request/").length());
        String responseTopic = "v1/devices/me/rpc/response/" + requestId;
        String payload = "{\"autoFlash\":" + String(autoFlashFirmware ? "true" : "false") + "}";
        client.publish(responseTopic.c_str(), payload.c_str());

    // Set auto flash value RPC Switch
    } else if (methodName.equals(("setAutoFlashValue"))) {
        JsonObject params = doc["params"];
        if (params.containsKey("autoFlash")) {
            autoFlashFirmware = params["autoFlash"];

            String requestId = topic.substring(String("v1/devices/me/rpc/request/").length());
            String responseTopic = "v1/devices/me/rpc/response/" + requestId;
            client.publish(responseTopic.c_str(), "{\"status\":\"Auto flash value updated\"}");
        }
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

String extractRpcMethodName(const String& payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return "";
    }
    
    if (doc.containsKey("method")) {
        return doc["method"].as<String>();
    } else {
        Serial.println("RPC request does not contain 'method' field");
        return "";
    }
}