// Render the actual example with the real display and graphics libraries.
#define setup greetingSetup
#define loop greetingLoop
#include "../../OLED/OLED.ino"
#undef setup
#undef loop

const char* layoutResult = "ERROR: layout test did not run";

void setup() {
  greetingSetup();
  if (!display.getBuffer()) {
    layoutResult = "ERROR: OLED unavailable; check wiring and reset";
    return;
  }

  GFXcanvas1 expected(64, 32);
  if (!expected.getBuffer()) {
    layoutResult = "ERROR: expected canvas allocation failed";
    return;
  }
  expected.fillScreen(0);
  expected.setTextColor(1);
  expected.setTextSize(2);
  expected.setCursor(2, 0);
  expected.print("Hello");
  expected.setCursor(2, 16);
  expected.print("OLED!");

  for (int16_t y = 0; y < 32; y++) {
    for (int16_t x = 0; x < 64; x++) {
      if (display.getPixel(x, y) != expected.getPixel(x, y)) {
        layoutResult = "FAIL: greeting bitmap mismatch";
        return;
      }
    }
  }
  layoutResult = "PASS: centered 2x greeting matches all 2048 pixels";
}

void loop() {
  Serial.println(layoutResult);
  delay(1000);
}
