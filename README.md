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
- [OLED](OLED/README.md): displays a greeting on a 64x32 I2C SSD1306 OLED.
- [MQTTLed](MQTTLed/README.md): controls the onboard LED with ON/OFF messages from a local MQTT broker.

## Compile and upload with Make

The root Makefile supports all examples. Run from the repository root:

```sh
make compile EXAMPLE=Blink
make upload EXAMPLE=WiFiScanner
make upload EXAMPLE=MQTTLed PORT=<port>
```

Set `EXAMPLE` to `Blink`, `HelloSerial`, `WiFiScanner`, `BLEScanner`, `OLED`, or
`MQTTLed`. It defaults to `MQTTLed`, so `make compile` and `make upload` also work
without an `EXAMPLE` argument. Configure `MQTTLed/Config.h` and install
PubSubClient as described in [MQTTLed/README.md](MQTTLed/README.md) before using
that example. For `OLED`, install the Adafruit dependencies as described in
[OLED/README.md](OLED/README.md).

`make upload` always compiles first. Without `PORT`, it auto-selects only when
Arduino CLI lists one ESP32 device; otherwise it asks you to specify the port.
Find the port with `arduino-cli board list` and pass `PORT=<port>` explicitly.
Run `make help` for usage. The original Arduino CLI commands remain available
in each example's README.

## Troubleshooting

### Serial monitor is empty

The XIAO FQBN used in this repository enables USB CDC by default. If you compile for the generic ESP32-C6 board instead, enable USB CDC so `Serial` is sent to the USB port:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32c6:CDCOnBoot=cdc <sketch-directory>
```
