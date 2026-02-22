#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f1xx_hal.h"
#include "stdint.h"

void flash_erase(uint32_t address, uint32_t numberOfPages);
void flash_write(uint32_t address, uint8_t *data, uint32_t length);
void flash_read(uint32_t address, uint8_t *buffer, uint32_t length);

#endif