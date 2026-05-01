#include <Arduino.h>

void showLabel(String label) {
  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(label);
  Serial2.write(0x03);
  delay(2000);
}

void trySend(String label, String pre, String post) {
  showLabel(label);
  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(pre);
  Serial2.print("HELLO WORLD");
  Serial2.print(post);
  Serial2.write(0x03);
  delay(4000);
}

void trySendRaw(String label, std::function<void()> fn) {
  showLabel(label);
  fn();
  delay(4000);
}

void setup() {
  Serial2.begin(9600, SERIAL_8N1, -1, 22, true);
  delay(5000);
}

void loop() {
  // --- BATCH 1: SOH + single char command + SCL variants ---
  trySend("B101", "\x01TA\x1BSCL", "");
  trySend("B102", "\x01T\x1BSCL", "");
  trySend("B103", "\x01A\x1BSCL", "");
  trySend("B104", "\x01\x1BSCL", "");
  trySend("B105", "\x01TA\x1BSCR", "");
  trySend("B106", "\x01TA\x1BROT", "");
  trySend("B107", "\x01TA\x1BSCL0", "");
  trySend("B108", "\x01TA\x1BSCL1", "");

  // --- BATCH 2: No SOH, just effect codes ---
  trySend("B201", "\x1BSCL", "");
  trySend("B202", "\x1BSCR", "");
  trySend("B203", "\x1BROT", "");
  trySend("B204", "\x1BMAR", "");
  trySend("B205", "\x1BSCL0", "");
  trySend("B206", "\x1BSCL1", "");
  trySend("B207", "\x1BSCLU", "");
  trySend("B208", "\x1BSCLD", "");

  // --- BATCH 3: Effect code AFTER text ---
  trySend("B301", "", "\x1BSCL");
  trySend("B302", "", "\x1BSCR");
  trySend("B303", "", "\x1BROT");
  trySend("B304", "A", "\x1BSCL");
  trySend("B305", "TA", "\x1BSCL");

  // --- BATCH 4: Single control bytes as prefix ---
  trySend("B401", "\x06", "");   // ACK
  trySend("B402", "\x07", "");   // BEL
  trySend("B403", "\x0C", "");   // FF
  trySend("B404", "\x0E", "");   // SO
  trySend("B405", "\x0F", "");   // SI
  trySend("B406", "\x10", "");   // DLE
  trySend("B407", "\x11", "");   // DC1
  trySend("B408", "\x12", "");   // DC2
  trySend("B409", "\x13", "");   // DC3
  trySend("B410", "\x14", "");   // DC4
  trySend("B411", "\x15", "");   // NAK
  trySend("B412", "\x16", "");   // SYN
  trySend("B413", "\x17", "");   // ETB
  trySend("B414", "\x18", "");   // CAN
  trySend("B415", "\x19", "");   // EM
  trySend("B416", "\x1A", "");   // SUB
  trySend("B417", "\x1C", "");   // FS
  trySend("B418", "\x1D", "");   // GS
  trySend("B419", "\x1E", "");   // RS
  trySend("B420", "\x1F", "");   // US

  // --- BATCH 5: Full MCS style with STX inside ---
  trySendRaw("B501", []() {
    Serial2.write(0x01); Serial2.print("01"); Serial2.write(0x02);
    Serial2.print("TA\x1BSCL"); Serial2.print("HELLO WORLD");
    Serial2.write(0x03);
  });
  trySendRaw("B502", []() {
    Serial2.write(0x02); Serial2.print("01"); Serial2.write(0x01);
    Serial2.print("TA"); Serial2.write(0x02);
    Serial2.print("\x1BSCL"); Serial2.print("HELLO WORLD");
    Serial2.write(0x04); Serial2.write(0x03);
  });

  // --- BATCH 6: Broadcast address 00 with scroll ---
  trySendRaw("B601", []() {
    Serial2.write(0x02); Serial2.print("00");
    Serial2.print("\x1BSCL"); Serial2.print("HELLO WORLD");
    Serial2.write(0x03);
  });
  trySendRaw("B602", []() {
    Serial2.write(0x02); Serial2.print("00");
    Serial2.write(0x01); Serial2.print("TA\x1BSCL");
    Serial2.print("HELLO WORLD");
    Serial2.write(0x03);
  });

  // --- BATCH 7: "$SCL$" format (some EDI docs use $ delimiters) ---
  trySend("B701", "$SCL$", "");
  trySend("B702", "$SCL$", "$");
  trySend("B703", "", "$SCL$");
  trySend("B704", "$", "");
  trySend("B705", "A$SCL$", "");

  // --- BATCH 8: ^G control code (effect code from MCS doc) ---
  trySend("B801", "\x07", "");
  trySend("B802", "\x01T\x07SCL", "");
  trySend("B803", "\x07SCL", "");
  trySend("B804", "\x07SCROLL", "");
  trySend("B805", "\x05SCL", "");   // ENQ + SCL

  delay(10000); // pause before looping
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