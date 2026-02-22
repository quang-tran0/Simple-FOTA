#ifndef __DOWNLOAD_FIRMWARE_TASK_H
#define __DOWNLOAD_FIRMWARE_TASK_H

#include "global.h"
#include <HTTPClient.h>

#define MAX_FW_SIZE (128 * 1024)

void downloadFirmwareTask(void *pvParameters);

#endif