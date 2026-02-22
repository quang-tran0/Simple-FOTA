#ifndef __FLASH_FIRMWARE_TASK_H
#define __FLASH_FIRMWARE_TASK_H

#include "global.h"

#define START_BYTE 0xAA          
#define ACK 0xCC
#define NACK 0x33
#define BLOCK_SIZE 256

void flashFirmwareTask(void *pvParameters);

#endif