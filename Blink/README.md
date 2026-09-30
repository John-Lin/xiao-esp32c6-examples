# Blink

Blinks the XIAO ESP32C6 onboard user LED once per second.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 Blink
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:XIAO_ESP32C6 Blink
```

Use the USB port reported by `arduino-cli board list` if it differs.
