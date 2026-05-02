// main.cpp - WiFi, web server, and display orchestration

// ----- includes ----- //
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "display.h"
#include "time_manager.h"
#include "fun_features.h"


// ----- variables ----- //
const char* ssid     = "ESP_Displayy";
const char* password = "123456789";

WebServer server(80);
String currentText = "";

int currentDirection = 1;
bool showClock = false;
bool RestrictedWord = false;

int sliderVal = 0; // global variable to store slider value for speed control



// ----- word filter ----- //
bool wordContains(const String &text) {
  //String lower = text;
  //lower.toLowerCase();
  //return lower.indexOf("") >= 0;
  return false; // no restrictions for now
}

// ----- file server ----- //
// Serves a file from SPIFFS (the ESP's internal flash storage)
void handleFile(String path, String type) {
  if (!SPIFFS.exists(path)) {
    server.send(404, "text/plain", "Missing file: " + path);
    return;
  }
  File file = SPIFFS.open(path, "r");
  server.streamFile(file, type);
  file.close();
}

// ----- scroll task ----- //
// Runs forever on core 0, separate from the web server so scrolling never blocks incoming requests
void scrollTask(void* param) {
  while (true) {
    if (!inActiveTime()) {
      sendFrame("      "); // blank when not in active hours
      delay(1000);
      continue;
    }
    if (showClock && isTimeSynced()) {
      sendFrame(" " + getCurrentTimeString().substring(0, 5)); // show "HH:MM"
      delay(500);
      continue;
    }
    if (currentText.length() > 0) {
      scrollText(currentText, currentDirection, 1);
    }
    delay(10);
  }
}

// ----- setup ----- //
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, -1, DISP_TX, true); // UART to the LED matrix

  if (!SPIFFS.begin(true)) {
    return;
  }
  WiFi.softAP(ssid, password); // create a WiFi access point

  xTaskCreatePinnedToCore(scrollTask, "scroll", 4096, NULL, 1, NULL, 0);


  // ----- routes ----- //
  // Each route maps a URL the browser can call to a function that handles it
  server.on("/", HTTP_GET, []() {
    handleFile("/index.html", "text/html");
  });

  server.on("/app.js", HTTP_GET, []() {
    handleFile("/app.js", "application/javascript");
  });

  server.on("/style.css", HTTP_GET, []() {
    handleFile("/style.css", "text/css");
  });

  server.on("/send.png", HTTP_GET, []() {
    handleFile("/send.png", "image/png");
  });


  // Receives text, speed, and direction from the web UI in one request
  server.on("/send", HTTP_GET, []() {
    // check for restricted words 
    if (server.hasArg("text")) {
      currentText = server.arg("text");
      RestrictedWord = wordContains(currentText);
    }
    // set the speed of the text
    if (server.hasArg("speed")) {
      sliderVal = server.arg("speed").toInt();
      ScrollSpeed = 200 - (150 * sliderVal) / 100; // maps 0-100% slider to 200-50ms delay
      if (ScrollSpeed < 50) ScrollSpeed = 50;       // clamp to min 50
    }
    // set the direction of the text
    if (server.hasArg("direction")) {
      currentDirection = server.arg("direction").toInt();
      if (currentDirection != 1 && currentDirection != -1) currentDirection = 1;
    }
    server.send(200, "text/plain", "OK");
  });


  // Receives speed from web
  server.on("/speed", HTTP_GET, []() {
    if (server.hasArg("speed")) {
      sliderVal = server.arg("speed").toInt();
      ScrollSpeed = 200 - (150 * sliderVal) / 100;
      if (ScrollSpeed < 50) ScrollSpeed = 50;
    }
    server.send(200, "text/plain", "OK");
  });

  
  // Receives the browser's Unix timestamp to sync the ESP's clock
  server.on("/sync", HTTP_GET, []() {
    if (server.hasArg("ts")) {
      syncTime((unsigned long)server.arg("ts").toInt());
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/time", HTTP_GET, []() {
    server.send(200, "text/plain", getCurrentTimeString());
  });

  server.on("/clock", HTTP_GET, []() {
    showClock = !showClock;
    server.send(200, "text/plain", showClock ? "1" : "0");
  });

  randomSeed(analogRead(0));
  server.begin();
}

// ----- loop ----- //
// The web server runs here on core 1 — handleClient() processes any incoming requests
void loop() {
  server.handleClient();
}
