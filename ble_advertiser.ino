#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void setup() {
  Serial.begin(115200);

  // BLE 초기화 및 디바이스 이름 설정
  BLEDevice::init("ESP32-OPTIMIZED");

  // 서버 객체 생성
  BLEServer *pServer = BLEDevice::createServer();

  // 광고 데이터 생성
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData advData;
  advData.setName("ESP32-ADV");
  advData.setCompleteServices(BLEUUID("180D"));  // Heart Rate 예시 UUID

  pAdvertising->setAdvertisementData(advData);

  // ✅ 최적화 설정들
  pAdvertising->setScanResponse(false);         // 스캔 응답 비활성화 (필요 없으면)
  pAdvertising->setMinInterval(0x20);           // 최소 인터벌 (20ms) = 0x0020
  pAdvertising->setMaxInterval(0x40);           // 최대 인터벌 (40ms) = 0x0040

  // ✅ 전송 전력 설정 (광고, 연결 등 유형별 모두 적용)
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);       // 광고
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);      // 스캔 응답
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_P9); // 연결된 핸들 (예시)

  BLEDevice::startAdvertising();
  Serial.println("BLE advertising with optimized settings...");
}

void loop() {
  delay(2000);
}  
