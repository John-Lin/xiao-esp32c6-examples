# WiFiScanner

Scans nearby Wi-Fi networks every 10 seconds. It does not connect to a network.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 WiFiScanner
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:XIAO_ESP32C6 WiFiScanner
arduino-cli monitor -p /dev/cu.usbmodem1101 --config baudrate=115200
```

Use the USB port reported by `arduino-cli board list` if it differs.
