#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "display.h"

const char* ssid = "ESP_Display";
const char* password = "12345678";

WebServer server(80);
String currentText = "";

// -------------------- SCROLL TASK --------------------
void scrollTask(void* param) {
  while (true) {
    if (currentText.length() > 0) {
      scrollText(currentText, 1, 1); // 1 loop at a time, task repeats forever
    }
    delay(10);
  }
}

// -------------------- FILE SERVER --------------------
void handleFile(String path, String type) {
  if (!SPIFFS.exists(path)) {
    server.send(404, "text/plain", "Missing file: " + path);
    return;
  }
  File file = SPIFFS.open(path, "r");
  server.streamFile(file, type);
  file.close();
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, -1, 22, true);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }
  Serial.println("SPIFFS OK");

  WiFi.softAP(ssid, password);
  Serial.println("WiFi started");

  xTaskCreatePinnedToCore(scrollTask, "scroll", 4096, NULL, 1, NULL, 0); // runs on core 0

  // -------------------- ROUTES --------------------
  server.on("/", HTTP_GET, []() {
    handleFile("/index.html", "text/html");
  });

  server.on("/app.js", HTTP_GET, []() {
    handleFile("/app.js", "application/javascript");
  });

  server.on("/style.css", HTTP_GET, []() {
    handleFile("/style.css", "text/css");
  });

  server.on("/send", HTTP_GET, []() {
    if (server.hasArg("text")) {
      currentText = server.arg("text");
      Serial.println("Got: " + currentText);
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started");
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();
}