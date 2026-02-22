#ifndef __MAIN_SERVER_H
#define __MAIN_SERVER_H

#include "global.h"
#include "uart.h"

extern const char *ssid;
extern const char *password;
extern WebServer server;

void main_server_task();

#endif