# OLED layout test

Run this hardware test after changing the greeting's size or position. It executes
`OLED/OLED.ino` on the real board, then compares every framebuffer pixel with a
centered, two-line, 2x greeting rendered by the real Adafruit GFX library.
No display or graphics mocks are used. It does not verify the physical panel;
visually check the screen as well.

Connect the OLED before powering up. From the repository root:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 tests/OLEDLayout
arduino-cli board list
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 tests/OLEDLayout
arduino-cli monitor -p <port> --config baudrate=115200
```

Replace `<port>` with the detected port. The monitor repeatedly reports
`PASS: centered 2x greeting matches all 2048 pixels` on success. `FAIL` indicates
a rendering mismatch; `ERROR` indicates the test could not run. Close the monitor
and restore the normal example afterwards:

```sh
make upload EXAMPLE=OLED PORT=<port>
```
