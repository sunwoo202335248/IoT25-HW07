#include <BLEDevice.h>
#include <BLEScan.h>
#include <math.h>

int scanTime = 5; // 스캔 시간 (초)
BLEScan* pBLEScan;

float rssiToDistance(int rssi) {
  int txPower = -59; // 1m 기준 RSSI 예상값
  if (rssi == 0) return -1.0;

  float ratio = rssi * 1.0 / txPower;
  if (ratio < 1.0) {
    return pow(ratio, 10);
  } else {
    float distance = 0.89976 * pow(ratio, 7.7095) + 0.111;
    return distance;
  }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    String targetName = "ESP32-ADV";
    if (advertisedDevice.getName() == targetName) {
      int rssi = advertisedDevice.getRSSI();
      Serial.print("Found ");
      Serial.print(targetName);
      Serial.print(" - RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm, Estimated Distance: ");

      float distance = rssiToDistance(rssi);
      if (distance >= 0) {
        Serial.print(distance);
        Serial.println(" meters");
      } else {
        Serial.println("Unknown");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {
  Serial.println("Scanning…");
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  pBLEScan->clearResults();
  delay(2000);
}
