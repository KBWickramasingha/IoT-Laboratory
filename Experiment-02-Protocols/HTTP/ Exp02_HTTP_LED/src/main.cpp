#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ── Change these to your Wi-Fi details ──
const char* ssid = "KBWick";
const char* password = "12345678";

// GPIO 2 is the onboard LED on most ESP32 boards
const int LED_PIN = 2;

// Create a web server listening on port 80
// Port 80 is the default HTTP port — no need to type :80 in browser
WebServer server(80);

// ── This function runs when browser visits http://[IP]/on ──
void handleOn() {
  digitalWrite(LED_PIN, HIGH);   // Turn LED on
  server.send(200, "text/plain", "LED is ON");
  Serial.println("Command received: LED ON");
}

// ── This function runs when browser visits http://[IP]/off ──
void handleOff() {
  digitalWrite(LED_PIN, LOW);    // Turn LED off
  server.send(200, "text/plain", "LED is OFF");
  Serial.println("Command received: LED OFF");
}

// ── This runs when browser visits http://[IP]/ (root) ──
void handleRoot() {
  // Send a simple HTML page with two buttons
  String html = "<h1>ESP32 LED Control</h1>";
  html += "<a href='/on'><button>LED ON</button></a> ";
  html += "<a href='/off'><button>LED OFF</button></a>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // Start with LED off

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  // Wait until connected — each dot = 500ms waiting
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print the IP address — you need this for the browser!
  Serial.println("\nConnected!");
  Serial.print("Open this in your browser: http://");
  Serial.println(WiFi.localIP());

  // Register which function handles which URL
  server.on("/",    handleRoot);
  server.on("/on",  handleOn);
  server.on("/off", handleOff);

  // Start the server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // This line checks for incoming browser requests every loop
  // Without this, the server won't respond to anything
  server.handleClient();
}