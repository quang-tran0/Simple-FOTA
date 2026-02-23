#include "flash_firmware_task.h"

uint16_t simpleChecksum(uint8_t *data, int len) {
    uint16_t crc = 0;
    for (int i = 0; i < len; i++)
        crc += data[i];
    return crc;
}

bool waitForAck(uint32_t timeout = 1000) {
    uint32_t start = millis();
    while (millis() - start < timeout) {
        if (Serial2.available()) {
            uint8_t resp = Serial2.read();
            if (resp == ACK) return true;
            else if (resp == NACK) {
                Serial.println("Received NACK!");
                return false;
            }
        }
    }
    return false;
}

void sendFirmwareToSTM32() {
    File file = SPIFFS.open("/firmware.bin", FILE_READ);
    if (!file) {
        Serial.println("Cant open /firmware.bin");
        return;
    }

    uint8_t data[BLOCK_SIZE];
    uint8_t packet[BLOCK_SIZE + 5];
    int blockCount = 0;

    while (file.available()) {
        int len = file.read(data, BLOCK_SIZE);
        uint16_t crc = simpleChecksum(data, len);

        packet[0] = START_BYTE;
        packet[1] = (len >> 8) & 0xFF;
        packet[2] = len & 0xFF;
        memcpy(&packet[3], data, len);
        packet[3 + len] = (crc >> 8) & 0xFF;
        packet[4 + len] = crc & 0xFF;
        Serial.printf("packet[0]: %02X, packet[1]: %02X, packet[2]: %02X, crc: %04X\n", packet[0], packet[1], packet[2], crc);

        int retry = 3;
        while (retry--) {
            while(Serial2.available()) Serial2.read();

            Serial2.write(packet, len + 5);
            Serial.printf("Sent block %d, waiting for ACK...\n", blockCount);

            if (waitForAck(1000)) {
                Serial.printf("Block %d: OK\n", blockCount);
                break;
            } else {
                Serial.printf("Block %d: NACK or timeout. Retrying...\n", blockCount);
            }
        }

        if (retry < 0) {
            Serial.println("Too many failed attempts. Aborting.");
            file.close();
            return;
        }

        blockCount++;
        delay(10);
    }

    // Send an empty packet to signal end of transmission
    uint8_t endPacket[5] = {START_BYTE, 0x00, 0x00, 0x00, 0x00};
    Serial2.write(endPacket, 5);

    file.close();
    Serial.println("Finished sending firmware.bin to STM32.");
}

void flashFirmwareTask(void *pvParameters) {
    while (1) {
        if (pendingFirmwareFlash) {
            Serial.println("Starting firmware flashing process...");
            sendFirmwareToSTM32();
            pendingFirmwareFlash = false;
        }
        vTaskDelay(5000);
    }
}