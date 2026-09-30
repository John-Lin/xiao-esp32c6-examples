#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

class AdvertisedDeviceReporter : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("%s\n", advertisedDevice.toString().c_str());
  }
};

BLEScan *scanner;

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");

  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceReporter());
  scanner->setActiveScan(true);
  scanner->setInterval(100);
  scanner->setWindow(99);
}

void loop() {
  Serial.println("Scanning Bluetooth LE devices...");
  BLEScanResults *devices = scanner->start(5, false);
  Serial.printf("Found %d device(s).\n\n", devices->getCount());
  scanner->clearResults();
  delay(2000);
}
