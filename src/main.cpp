#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

#define DISP_TX 17
#define DISP_RX 16

const char* ssid = "ESP_Display";
const char* password = "12345678";

WebServer server(80);

// -------------------- DISPLAY --------------------
void sendToDisplay(String text) {
  Serial2.write(0x02);
  Serial2.print(text);
  Serial2.write(0x03);
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

  Serial2.begin(9600, SERIAL_8N1, DISP_RX, DISP_TX);

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }
  Serial.println("SPIFFS OK");

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi started");

  // ROUTES
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
      String text = server.arg("text");
      sendToDisplay(text);
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