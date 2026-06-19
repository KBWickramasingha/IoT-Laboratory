#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ── Wi-Fi credentials ──
const char* WIFI_SSID     = "";
const char* WIFI_PASSWORD = "";

// ── Your laptop's IP address (the one from ipconfig) ──
const char* MQTT_BROKER = "192.168.1.103";   // CHANGE THIS to your laptop's IP
const int   MQTT_PORT   = 1883;

// ── MQTT topics ──
// "Topic" = like a channel name. ESP32 listens on this one.
const char* TOPIC_COMMAND = "lab/esp32/led";
const char* TOPIC_STATUS  = "lab/esp32/led/status";

const int LED_PIN = 2;

WiFiClient   espClient;
PubSubClient mqttClient(espClient);

// ── This function runs automatically whenever a message arrives ──
// on a topic we've subscribed to
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // payload arrives as raw bytes — convert to a String
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(" → ");
  Serial.println(message);

  if (message == "ON") {
    digitalWrite(LED_PIN, HIGH);
    mqttClient.publish(TOPIC_STATUS, "ON");   // tell Node-RED the new state
  } else if (message == "OFF") {
    digitalWrite(LED_PIN, LOW);
    mqttClient.publish(TOPIC_STATUS, "OFF");
  }
}

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected! IP: " + WiFi.localIP().toString());
}

void connectMQTT() {
  // Keep trying until connected
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");
    // "ESP32Client" is just a unique name for this device on the broker
    if (mqttClient.connect("ESP32Client")) {
      Serial.println(" connected!");
      mqttClient.subscribe(TOPIC_COMMAND);  // start listening on this topic
      Serial.print("Subscribed to: ");
      Serial.println(TOPIC_COMMAND);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" — retrying in 3 seconds");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  connectWiFi();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);  // tell it which function handles incoming messages

  connectMQTT();
}

void loop() {
  // Reconnect automatically if dropped
  if (!mqttClient.connected()) {
    connectMQTT();
  }

  // This MUST be called repeatedly — it checks for incoming messages
  // and keeps the connection alive
  mqttClient.loop();
}