#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D5);  // SDA = GPIO22, SCL = GPIO23 on the XIAO ESP32C6.

  // The display library does not check whether an I2C device responds.
  Wire.beginTransmission(OLED_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.printf("No I2C device at 0x%02X. Check OLED wiring and power.\n", OLED_ADDR);
    return;
  }

  // Keep the I2C bus initialized with the pins selected above.
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR, false, false)) {
    Serial.println("OLED buffer allocation failed.");
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // The full greeting is wider than 64 pixels in the default font.
  display.println("Hello,");
  display.println("OLED!");
  display.display();
  Serial.printf("OLED frame sent to 0x%02X.\n", OLED_ADDR);
}

void loop() {
  delay(1000);
}
