#include "download_firmware_task.h"

String getFirmwareUrl(String title, String version = "1.0.0") {
    return String("https://") + SERVER_URL + "/api/v1/" + DEVICE_TOKEN + "/firmware?title=" + title + "&version=" + version;
}

void downloadFirmware() {
    String url = getFirmwareUrl(currentFirmwareTitle, currentFirmwareVersion);
    Serial.println("Downloading firmware from: " + url);

    SPIFFS.remove("/firmware.bin");
    File firmwareFile = SPIFFS.open("/firmware.bin", FILE_WRITE);
    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();

        if (contentLength <= MAX_FW_SIZE) {
            WiFiClient *stream = http.getStreamPtr();
            int bytesRead = 0;

            while (http.connected() && bytesRead < contentLength) {
                if (stream->available()) {
                    firmwareFile.write(stream->read());
                    bytesRead++;
                }

                if (bytesRead % 1024 == 0) {
                    yield();
                    Serial.printf("Downloaded %d/%d bytes\n", bytesRead, contentLength);
                }
            }

            firmwareFile.close();

            if (bytesRead == contentLength) {
                Serial.println("Firmware download complete. Size: " + String(bytesRead) + " bytes");
            } else {
                Serial.println("Firmware download incomplete. Expected: " + String(contentLength) + " bytes, Got: " + String(bytesRead) + " bytes");
            }
        } else {
            Serial.println("Firmware size exceeds maximum allowed size of " + String(MAX_FW_SIZE) + " bytes");
        }
    } else {
        Serial.println("Failed to download firmware. HTTP code: " + String(httpCode));
    }

    // check if firmware file exists and is valid
    if (SPIFFS.exists("/firmware.bin")) {
        File f = SPIFFS.open("/firmware.bin");
        if (f && f.size() > 0) {
            Serial.println("Firmware file is valid. Size: " + String(f.size()) + " bytes");
            f.close();
        } else {
            Serial.println("Firmware file is invalid or empty.");
        }
    } else {
        Serial.println("Firmware file does not exist.");
    }
    http.end();
}

void downloadFirmwareTask(void *pvParameters) {
    while (1) {
        if (pendingFirmwareDownload) {
            Serial.println("Starting firmware download process...");
            downloadFirmware();
            pendingFirmwareDownload = false;
        }
        vTaskDelay(5000);
    }
}