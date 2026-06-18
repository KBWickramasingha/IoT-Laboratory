#include <Arduino.h>
#include <WiFi.h>

// 🔧 Change these to your hotspot details
const char* ssid = "KBWick";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n🚀 ESP32 WiFi Connection Test");

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n\n✅ WiFi Connected!");
  Serial.print("📡 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Just keep checking connection
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("🟢 Connected to WiFi");
  } else {
    Serial.println("🔴 Disconnected");
  }

  delay(3000);
}