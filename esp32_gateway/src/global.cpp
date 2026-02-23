#include "global.h"

// ########################################
// # Configuration
// ########################################

const char* WIFI_SSID = "MAY CAY HUY PHU";
const char* WIFI_PASSWORD = "12345678";

const char* DEVICE_TOKEN = "4R5f98rUJ7B3eZuCADWP";
const char* SERVER_URL = "app.coreiot.io";



// ########################################
// # Global Variables
// ########################################

bool pendingFirmwareDownload = false;
bool pendingFirmwareFlash = false;
bool autoFlashFirmware = false;
String currentFirmwareVersion = "";
String currentFirmwareTitle = "";