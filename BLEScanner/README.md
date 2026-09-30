# BLEScanner

Scans nearby Bluetooth LE advertisements every 5 seconds.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 BLEScanner
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:XIAO_ESP32C6 BLEScanner
arduino-cli monitor -p /dev/cu.usbmodem1101 --config baudrate=115200
```

Use the USB port reported by `arduino-cli board list` if it differs.
