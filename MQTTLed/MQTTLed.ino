#include <WiFi.h>
#include <PubSubClient.h>

#include "Config.h"
#include "LedCommand.h"

const uint16_t mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void handleMessage(char* topic, uint8_t* payload, unsigned int length) {
  int level = ledLevelForMessage(topic, payload, length);
  if (level < 0) {
    Serial.println("Ignored MQTT message: expected ON or OFF on xiao/led/set.");
    return;
  }

  digitalWrite(LED_BUILTIN, level);
  Serial.println(level == LOW ? "LED ON" : "LED OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // The onboard user LED is active-low.

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  mqttClient.setServer(mqttHost, mqttPort);
  mqttClient.setCallback(handleMessage);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    mqttClient.disconnect();
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(wifiSsid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
  }

  if (!mqttClient.connected()) {
    // A MAC-based ID prevents different boards from disconnecting each other.
    String clientId = "xiao-c6-" + WiFi.macAddress();
    Serial.println("Connecting to MQTT...");
    if (!mqttClient.connect(clientId.c_str())) {
      Serial.printf("MQTT connection failed (state %d). Retrying in 5 seconds.\n",
                    mqttClient.state());
      delay(5000);
      return;
    }
    if (!mqttClient.subscribe(ledTopic)) {
      Serial.println("MQTT subscription failed. Retrying in 5 seconds.");
      mqttClient.disconnect();
      delay(5000);
      return;
    }
    Serial.print("MQTT connected. Subscribed to ");
    Serial.println(ledTopic);
  }

  mqttClient.loop();
  delay(10);
}
