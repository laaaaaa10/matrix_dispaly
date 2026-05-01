#include <Arduino.h>

void send(String label, String payload) {
  // show label first
  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(label);
  Serial2.write(0x03);
  delay(3000);

  // send test
  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(payload);
  Serial2.write(0x03);
  delay(4000);
}

void setup() {
  Serial2.begin(9600, SERIAL_8N1, -1, 22, true);
  delay(5000);
}

void loop() {
  // try different effect codes before text
  send("T01", "\x1BSCL" "HELLO WORLD");
  send("T02", "\x1BSCR" "HELLO WORLD");
  send("T03", "\x1BROT" "HELLO WORLD");
  send("T04", "\x1BSCL0" "HELLO WORLD");
  send("T05", "\x01" "A\x1BSCL" "HELLO WORLD");
  send("T06", "\x01" "A" "HELLO WORLD");  // no effect, just MCS
  send("T07", "\x02" "A\x1BSCL" "HELLO WORLD");
  send("T08", "A\x1BSCL" "HELLO WORLD");
  send("T09", "\x1BSCLU" "HELLO WORLD");
  send("T10", "\x06" "HELLO WORLD");  // ACK prefix
}

/*



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
  Serial2.print("01");
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

  Serial2.begin(9600, SERIAL_8N1, -1, 22, true);
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
      Serial.println("Got: " + text);  // check serial monitor
      Serial2.write(0x02);
      Serial2.print("01");
      Serial2.print(text);
      Serial2.write(0x03);
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started");
}


// -------------------- LOOP --------------------
void loop() {
  server.handleClient();
}*/