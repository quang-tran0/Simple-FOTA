#include "wifi_task.h"

void wifiTask(void *pvParameters) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("WiFi connecting...");
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected. IP:");
    Serial.println(WiFi.localIP());

    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected. Reconnecting...");
            WiFi.reconnect();
            vTaskDelay(2000);
        }
    }
}