# ESP32-C6 Arduino CLI Setup

This workspace uses Arduino CLI with the Espressif Arduino core. Arduino IDE is not required.

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

The included `HelloSerial` demo targets `esp32:esp32:esp32c6`:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32c6:CDCOnBoot=cdc HelloSerial
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:esp32c6:CDCOnBoot=cdc HelloSerial
arduino-cli monitor -p /dev/cu.usbmodem1101 --config baudrate=115200
```

Run `arduino-cli board list` to check the USB serial port before uploading.
