#include "global.h"

#include "coreiot_task.h"
#include "download_firmware_task.h"
#include "flash_firmware_task.h"
#include "wifi_task.h"

void setup() {
    Serial.begin(9600);
    Serial2.begin(115200);

    SPIFFS.begin(true);

    xTaskCreate(wifiTask, "WiFi Task", 4096, NULL, 1, NULL);
    xTaskCreate(coreiotTask, "CoreIoT Task", 4096, NULL, 1, NULL);
    xTaskCreate(downloadFirmwareTask, "Download Firmware Task", 8192, NULL, 1, NULL);
    xTaskCreate(flashFirmwareTask, "Flash Firmware Task", 8192, NULL, 1, NULL);
}

void loop() {}