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

## Build and upload

The included `HelloSerial` and `Blink` demos target `esp32:esp32:XIAO_ESP32C6`:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 HelloSerial
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:XIAO_ESP32C6 HelloSerial
arduino-cli monitor -p /dev/cu.usbmodem1101 --config baudrate=115200
```

Replace `HelloSerial` with `Blink` to flash the onboard LED.

Run `arduino-cli board list` to check the USB serial port before uploading.
