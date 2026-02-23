#ifndef __COREIOT_TASK_H
#define __COREIOT_TASK_H

#include "global.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void coreiotTask(void *pvParameters);
void coreiotSetup();
void coreiotConnect();
void callback(char* topic, byte* payload, unsigned int length);
void handleAttributeTopic(const String& topic, const String& message);
void handleRpcTopic(const String& topic, const String& message);
void extractFirmwareInfo(JsonObject obj);
String extractRpcMethodName(const String& payload);

#endif