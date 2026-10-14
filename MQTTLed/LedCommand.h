#pragma once

#include <cstdint>
#include <cstring>

constexpr char ledTopic[] = "xiao/led/set";

// The onboard LED is active-low: 0 is on, 1 is off, -1 leaves it unchanged.
inline int ledLevelForMessage(const char* topic, const uint8_t* payload,
                              unsigned int length) {
  if (std::strcmp(topic, ledTopic) != 0) {
    return -1;
  }
  // MQTT payloads are byte buffers, not null-terminated strings.
  if (length == 2 && std::memcmp(payload, "ON", 2) == 0) {
    return 0;
  }
  if (length == 3 && std::memcmp(payload, "OFF", 3) == 0) {
    return 1;
  }
  return -1;
}
