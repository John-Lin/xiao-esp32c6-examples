#!/usr/bin/env bash
# Generate private configuration, compile, and optionally upload the MQTT LED sketch.
set -euo pipefail

usage="Usage: scripts/build-mqtt-led.sh [--upload <port>]
Run with WIFI_SSID, WIFI_PASSWORD, and MQTT_HOST set (for example via direnv exec)."
if [[ $# -eq 1 && "$1" == "--help" ]]; then
  echo "$usage"
  exit 0
fi
if [[ $# -ne 0 ]] && [[ $# -ne 2 || "$1" != "--upload" || -z "${2:-}" ]]; then
  echo "$usage" >&2
  exit 2
fi

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
node "$root/scripts/generate-mqtt-config.mjs" "$root/MQTTLed/Config.h"
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 "$root/MQTTLed"
if [[ $# -eq 2 ]]; then
  arduino-cli upload -p "$2" --fqbn esp32:esp32:XIAO_ESP32C6 "$root/MQTTLed"
fi
