#ifndef __UART_H
#define __UART_H

#include "global.h"

#define START_BYTE 0xAA          
#define ACK 0xCC
#define NACK 0x33
#define BLOCK_SIZE 256

void sendFirmwareToSTM32();

#endif