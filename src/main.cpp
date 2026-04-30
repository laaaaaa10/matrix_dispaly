#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define DISP_TX 17
#define DISP_RX 16

const char* ssid = "ESP_Display";
const char* password = "12345678";

WebServer server(80);

// send text to your matrix (same protocol as before)
void sendToDisplay(String text) {
  Serial2.write(0x02);
  Serial2.print(text);
  Serial2.write(0x03);
}

// webpage
void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html>
    <body>
      <h2>ESP Matrix Control</h2>

      <input type="text" id="txt" placeholder="Type here">
      <button onclick="sendText()">Send</button>

      <script>
        function sendText() {
          let t = document.getElementById("txt").value;
          fetch("/send?text=" + encodeURIComponent(t));
        }
      </script>
    </body>
    </html>
  )rawliteral");
}

// receive text from webpage
void handleSend() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    sendToDisplay(text);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial2.begin(9600, SERIAL_8N1, DISP_RX, DISP_TX);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/send", handleSend);

  server.begin();
}

void loop() {
  server.handleClient();
}