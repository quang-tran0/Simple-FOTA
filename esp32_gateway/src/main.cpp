#include "global.h"

#include "uart.h"
#include "main_server.h"


void setup()
{
  Serial.begin(9600);
  Serial2.begin(115200);

  SPIFFS.begin(true);

  WiFi.begin(ssid, password);
  Serial.print("WiFi connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP:");
  Serial.println(WiFi.localIP());

  main_server_task();
}

void loop()
{
  server.handleClient();
}