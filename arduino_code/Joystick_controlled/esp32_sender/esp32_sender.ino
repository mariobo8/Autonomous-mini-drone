#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// Replace with the receiver's MAC address
uint8_t receiverMacAddress[][6] = {
  {0xD8, 0x13, 0x2A, 0x2E, 0xD1, 0x2C}
};

esp_now_peer_info_t peerInfo;

// Variables to store parsed data
bool armed = false;
int throttle = 0;
int pitch = 0;
int roll = 0;
int yaw = 0;

// LED settings
const int LED_PIN = 2;  // Built-in LED pin for most ESP32 boards
const int LED_BLINK_DURATION = 50;  // LED blink duration in milliseconds
unsigned long ledTurnOffTime = 0;

// Serial buffer
String inputBuffer = "";

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // We won't print anything here to avoid interfering with incoming serial data
}

void setup() {
    Serial.begin(1000000);  // Initialize serial communication
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    
    // Blink LED for 3 seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) {
        digitalWrite(LED_PIN, HIGH);
        delay(70);
        digitalWrite(LED_PIN, LOW);
        delay(70);
    }
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
      return;
    }

    // Register the send callback
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, receiverMacAddress[0], 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      return;
    }

    // Optional: Set ESP-NOW rate
    esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_24M);
}


char buffer[1024];
void loop() {
  int availableBytes = Serial.available();
  if (availableBytes) {
    Serial.readBytes(buffer, availableBytes);
    buffer[availableBytes] = '\0';
    esp_err_t result = esp_now_send(receiverMacAddress[0], (uint8_t *)&buffer, strlen(buffer));

    if (result == ESP_OK){ 
      Serial.println("Data sent successfully. Waiting for callback...");
      // Blink LED to indicate data was processed
      digitalWrite(LED_PIN, HIGH);
      ledTurnOffTime = millis() + LED_BLINK_DURATION;
    }

  }
  else {
    yield();
  }
    // Check if it's time to turn off the LED
  if (ledTurnOffTime > 0 && millis() >= ledTurnOffTime) {
    digitalWrite(LED_PIN, LOW);
    ledTurnOffTime = 0;  // Reset the turn-off time
  }
}