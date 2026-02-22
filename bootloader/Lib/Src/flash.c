#include "flash.h"

void flash_erase(uint32_t address, uint32_t numberOfPages) {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef eraseInitStruct;
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = address;
    eraseInitStruct.NbPages = numberOfPages;

    uint32_t pageError;
    HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
    HAL_FLASH_Lock();
}

void flash_write(uint32_t address, uint8_t *data, uint32_t length) {
    HAL_FLASH_Unlock();

    uint32_t numberOfPages = (length + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    uint32_t remainingBytes = length % 4;
    
    for (uint32_t i = 0; i < length - remainingBytes; i += 4) {
        uint32_t word = (data[i]) | (data[i + 1] << 8) | (data[i + 2] << 16) | (data[i + 3] << 24);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, word);
    }

    // Handle remaining bytes
    if (remainingBytes > 0) {
        uint32_t word = 0xFFFFFFFF;
        for (uint32_t i = 0; i < remainingBytes; i++) {
            word &= ~(0xFF << (i * 8));
            word |= (data[length - remainingBytes + i] << (i * 8));
        }
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + length - remainingBytes, word);
    }

    HAL_FLASH_Lock();
}

void flash_read(uint32_t address, uint8_t *buffer, uint32_t length) {
    for (uint32_t i = 0; i < length; i += 4) {
        uint32_t word = *(volatile uint32_t*)(address + (i & ~0x3));
        if (i < length) buffer[i] = (word >> 0) & 0xFF;
        if (i + 1 < length) buffer[i + 1] = (word >> 8) & 0xFF;
        if (i + 2 < length) buffer[i + 2] = (word >> 16) & 0xFF;
        if (i + 3 < length) buffer[i + 3] = (word >> 24) & 0xFF;
    }
}