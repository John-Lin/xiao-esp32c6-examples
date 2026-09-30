# ESP32-C6 Arduino CLI Setup

This workspace uses Arduino CLI with the Espressif Arduino core. Arduino IDE is not required.

## One-time setup

```sh
arduino-cli config init \
  --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json

arduino-cli core update-index
arduino-cli core install esp32:esp32
```

Confirm the installed core and list supported ESP32-C6 boards:

```sh
arduino-cli core list
arduino-cli board listall "ESP32C6 Dev Module"
```

## Connected board

The connected board is available on `/dev/cu.usbmodem1101`.

Use the generic ESP32-C6 FQBN `esp32:esp32:esp32c6` to compile and upload a sketch:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32c6 <sketch-directory>
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn esp32:esp32:esp32c6 <sketch-directory>
```

Use `arduino-cli board list` to check the current USB serial port before uploading.
