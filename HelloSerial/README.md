# HelloSerial

Prints `Hello from ESP32-C6` once per second over USB serial.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 HelloSerial
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 HelloSerial
arduino-cli monitor -p <port> --config baudrate=115200
```

Find `<port>` with `arduino-cli board list`.
