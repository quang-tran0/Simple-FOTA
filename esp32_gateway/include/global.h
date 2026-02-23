#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

extern const char* DEVICE_TOKEN;
extern const char* SERVER_URL;

extern bool pendingFirmwareDownload;
extern bool pendingFirmwareFlash;
extern bool autoFlashFirmware;
extern String currentFirmwareVersion;
extern String currentFirmwareTitle;

#endif