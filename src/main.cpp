#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "display.h"

const char* ssid = "ESP_Display";
const char* password = "12345678";

WebServer server(80);

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, -1, 22, true);
  SPIFFS.begin(true);
  WiFi.softAP(ssid, password);

  // -------------------- ROUTES --------------------
  server.on("/", HTTP_GET, []() {
    File f = SPIFFS.open("/index.html", "r");
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/send", HTTP_GET, []() {
    if (server.hasArg("text"))
      scrollText(server.arg("text"), 1, 3);
    server.send(200, "text/plain", "OK");
  });

  server.begin();
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();
}