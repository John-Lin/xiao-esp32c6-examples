#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.STA.begin();
}

void loop() {
  Serial.println("Scanning Wi-Fi networks...");
  int networkCount = WiFi.scanNetworks();

  if (networkCount == 0) {
    Serial.println("No networks found.");
  } else {
    for (int networkIndex = 0; networkIndex < networkCount; ++networkIndex) {
      Serial.printf("%d: %s (%ld dBm) channel %d\n", networkIndex + 1,
                    WiFi.SSID(networkIndex).c_str(), WiFi.RSSI(networkIndex),
                    WiFi.channel(networkIndex));
    }
  }

  WiFi.scanDelete();
  Serial.println();
  delay(10000);
}
