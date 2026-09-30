# Blink

Blinks the XIAO ESP32C6 onboard user LED once per second.

From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 Blink
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 Blink
```

Find `<port>` with `arduino-cli board list`.
