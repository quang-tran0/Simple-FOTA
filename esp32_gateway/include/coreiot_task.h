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

#endif