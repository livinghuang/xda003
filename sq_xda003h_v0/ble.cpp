#include "ble.h"
#include "nimble_src/NimBLEDevice.h"

#define SCAN_INTERVAL 100 // interval (milliseconds)
#define SCAN_WINDOW 90    // window (milliseconds)
#define SCAN_TIME 1000    // 1 seconds

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
  void onResult(NimBLEAdvertisedDevice *advertisedDevice)
  {
    // Optionally, do something with the device, but try to avoid serial print in low power mode.
    Serial.print("Device Found: ");
    Serial.println(advertisedDevice->getAddress().toString().c_str());
  }
};

void ble_init()
{
  Serial.begin(115200);
  Serial.println("Starting NimBLE BLE Scan...");
  NimBLEDevice::init("");

  NimBLEScan *pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(false); // Use passive scanning
  pBLEScan->setInterval(SCAN_INTERVAL);
  pBLEScan->setWindow(SCAN_WINDOW);

  // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->start(10000, false);

  // uint32_t time = millis();
  // while (pBLEScan->isScanning())
  // {
  //   if ((millis() - time) > 1000)
  //   {
  //     Serial.println("Scanning...");
  //     time = millis();
  //   }
  // }
  while (1)
  {
    Serial.println("Scan done!");
    delay(1000);
  }
}

bool is_ble_working()
{
  return NimBLEDevice::getScan()->isScanning();
}

void ble_close()
{
  NimBLEDevice::getScan()->clearResults();
  NimBLEDevice::getScan()->stop();
  NimBLEDevice::deinit();
}