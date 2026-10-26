# OLED

Displays `Hello` and `OLED!` on two lines of a 0.49-inch, 64x32 I2C SSD1306
OLED. The default font is scaled 2x: each character cell is 12x16 pixels, so
five characters occupy 60 pixels with a 2-pixel margin on either side. The two
lines use the full 32-pixel cell height, with normal spacing around the glyphs.
The text is drawn once and remains visible until power is removed or the board
resets. No separate display reset pin is used. Connect the OLED before power-up,
or press RESET after connecting it; initialization is not retried in `loop()`.

## Wiring

Disconnect power before changing wiring. Use a module that supports 3.3 V power
and I2C logic; do not pull the XIAO's GPIO pins up to 5 V.

| OLED | XIAO ESP32C6 |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| SDA | D4 / GPIO22 |
| SCL (sometimes labeled SCK) | D5 / GPIO23 |

The sketch uses I2C address `0x3C`. Change `OLED_ADDR` in `OLED.ino` if your
module uses another address.

## Install dependencies

From the repository root:

```sh
arduino-cli lib install "Adafruit SSD1306@2.5.17"
```

Arduino CLI also installs the required Adafruit GFX Library and Adafruit BusIO
dependencies. SSD1306 2.5.17 includes initialization and column-offset handling
for 64x32 displays.

## Compile and upload

From the repository root:

```sh
make compile EXAMPLE=OLED
arduino-cli board list
make upload EXAMPLE=OLED PORT=<port>
arduino-cli monitor -p <port> --config baudrate=115200
```

Replace `<port>` with the value reported by `arduino-cli board list`. Uploading
replaces the application currently on the board. Close the serial monitor before
uploading again.

The equivalent direct Arduino CLI commands are:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 OLED
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 OLED
```

## Verification

1. Run `make compile EXAMPLE=OLED`: it must compile for the XIAO ESP32C6 without
   errors.
2. Upload with the OLED connected: `Hello` appears on the first line and `OLED!`
   on the second, both at 2x size with matching 2-pixel cell margins. No characters
   are missing, clipped, or wrapped. The text remains visible without a serial
   monitor connected.
3. Open the serial monitor and reset the board: it reports
   `OLED frame sent to 0x3C.` This confirms the software reached the display
   transfer, not that the panel visibly rendered it; check the screen too.
4. Disconnect power, disconnect the OLED, then power up the XIAO. Open the serial
   monitor and reset: it reports `No I2C device at 0x3C. Check OLED wiring and power.`
   The board must not repeatedly reboot.
5. Disconnect power, reconnect the OLED, and power up again: the greeting returns.

If the screen is blank, check power, SDA/SCL orientation, and the address first.
An I2C acknowledgement only proves that a device responds at that address;
`display.begin()` does not verify that an SSD1306 panel is present. If an I2C
device responds but the screen is still blank, confirm the controller and 64x32
panel specifications. An `OLED buffer allocation failed.` message indicates
that the display buffer could not be allocated.

The [hardware layout test](../tests/OLEDLayout/README.md) checks all 2048
framebuffer pixels against the expected large-text layout.

## References

- [Product page](https://www.taiwaniot.com.tw/product/0-49-inch-micro-oled-display-64x32-i2c-ssd1306/)
- [XIAO ESP32C6 pin multiplexing](https://wiki.seeedstudio.com/xiao_pin_multiplexing_esp33c6/)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
