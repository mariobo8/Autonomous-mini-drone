#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

uint8_t broadcastAddresses[][6] = {
  {0x88, 0x13, 0xBF, 0x03, 0x91, 0x3C}
};

esp_now_peer_info_t peerInfo;

// Structure to hold our random data
struct RandomData {
  int droneIndex;
  float x;
  float y;
  float z;
};

const int LED_PIN = 2;  // Built-in LED pin for most ESP32 boards

// Global variable to track the last send status
volatile bool lastSendSuccess = false;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  for (int i = 0; i < 2; i++) {
    memcpy(peerInfo.peer_addr, broadcastAddresses[i], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }
  }

  Serial.println("ESP-NOW setup complete. Will start sending random data.");
}

void sendRandomData() {
  RandomData data;
  data.droneIndex = random(0, 2);  // Randomly choose drone 0 or 1
  data.x = random(-100, 100) / 10.0;  // Random float between -10.0 and 10.0
  data.y = random(-100, 100) / 10.0;
  data.z = random(-100, 100) / 10.0;

  Serial.printf("Sending to drone %d: x=%.1f, y=%.1f, z=%.1f\n", 
                data.droneIndex, data.x, data.y, data.z);

  lastSendSuccess = false;

  esp_err_t result = esp_now_send(broadcastAddresses[data.droneIndex], (uint8_t *)&data, sizeof(RandomData));
  
  if (result == ESP_OK) 
    digitalWrite(LED_PIN, HIGH);
    Serial.println("SEGNAME INVIATO CON SUCCESSO");
    
}

void loop() {
  sendRandomData();
  delay(5);
}