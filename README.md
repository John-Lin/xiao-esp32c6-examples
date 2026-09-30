# XIAO ESP32-C6 Arduino CLI Examples

This workspace targets the Seeed Studio XIAO ESP32C6 with Arduino CLI. Arduino IDE is not required.

## Hardware prerequisites

- Seeed Studio XIAO ESP32C6
- USB-C data cable

## One-time setup

```sh
arduino-cli config init \
  --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json

arduino-cli core update-index
arduino-cli core install esp32:esp32
```

## Apple Silicon

Arduino's `ctags` helper requires Rosetta 2:

```sh
softwareupdate --install-rosetta --agree-to-license
```

## Find the upload port

```sh
arduino-cli board list
```

Use the value in the `Port` column as `<port>` in upload commands. Common forms are `/dev/cu.usbmodem...` on macOS, `/dev/ttyACM0` or `/dev/ttyUSB0` on Linux, and `COM3` on Windows. The reported value is authoritative.

## Quick start

[Blink](Blink/README.md) is the simplest example. It blinks the onboard user LED.

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 Blink
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 Blink
```

## Examples

- [HelloSerial](HelloSerial/README.md): prints a message over USB serial.
- [Blink](Blink/README.md): blinks the onboard user LED.
- [WiFiScanner](WiFiScanner/README.md): scans nearby Wi-Fi networks.
- [BLEScanner](BLEScanner/README.md): scans nearby Bluetooth LE devices.
