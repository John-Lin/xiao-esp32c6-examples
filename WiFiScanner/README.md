# WiFiScanner

Scans nearby Wi-Fi networks every 10 seconds. It does not connect to a network.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 WiFiScanner
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 WiFiScanner
arduino-cli monitor -p <port> --config baudrate=115200
```

Find `<port>` with `arduino-cli board list`.
