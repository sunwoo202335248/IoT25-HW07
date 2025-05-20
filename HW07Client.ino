#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <WiFi.h>
#include <WebServer.h>
#define LED_PIN 2

const char* ssid = "syw";
const char* password = "syw80480929";

WebServer server(80);

BLEScan* pBLEScan;
int targetRSSI = 0;
String targetDeviceName = "ESP32-ADV";  

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName() && advertisedDevice.getName() == targetDeviceName) {
      targetRSSI = advertisedDevice.getRSSI();
      Serial.printf("Device found: %s, RSSI: %d\n", advertisedDevice.getName().c_str(), targetRSSI);
    }
  }
};

float estimateDistanceFromRSSI(int rssi) { // m 변환
  int txPower = -65;  
  float n = 2.5;
  return pow(10.0, (txPower - rssi) / (10.0 * n));
}

String classifyDistance(float distance) {
  if (distance <= 1.0) return "≤ 1m";
  else if (distance <= 2.0) return "> 1m";
  else if (distance <= 3.0) return "> 2m";
  else if (distance <= 4.0) return "> 3m";
  else if (distance <= 5.0) return "> 4m";
  else return "> 5m";
}

void handleRoot() {
  float distance = estimateDistanceFromRSSI(targetRSSI);
  String label = classifyDistance(distance);

  String html = "<html><head><meta http-equiv='refresh' content='2'></head><body>";
  html += "<h1>ESP32 BLE RSSI Monitor</h1>";
  html += "<h1>Device: " + targetDeviceName + "</h1>";
  html += "<h1>RSSI: " + String(targetRSSI) + " dBm</h1>";
  html += "<h1>Estimated Distance: " + String(distance, 2) + " m</h1>";  // 거리 출력
  html += "<h1>Range: " + label + "</h1>"; // 범위 출력
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // 웹서버 시작
  server.on("/", handleRoot);
  server.begin();

  // BLE 초기화
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(48);
  pBLEScan->setWindow(24);
}

void loop() {
  pBLEScan->start(3, false);

  // 거리 계산 및 LED 제어
  float distance = estimateDistanceFromRSSI(targetRSSI);
  if (distance <= 1.0) {
    digitalWrite(LED_PIN, HIGH);  // 1미터 이내면 LED ON
  } else {
    digitalWrite(LED_PIN, LOW);   // 아니면 OFF
  }

  server.handleClient();     
  delay(1000);                
}


