# BLEScanner

Scans nearby Bluetooth LE advertisements every 5 seconds.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 BLEScanner
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 BLEScanner
arduino-cli monitor -p <port> --config baudrate=115200
```

Find `<port>` with `arduino-cli board list`.
