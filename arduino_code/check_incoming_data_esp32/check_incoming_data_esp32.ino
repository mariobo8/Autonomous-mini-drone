#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>

// Structure to hold our random data (matching the sender's structure)
struct RandomData {
  int droneIndex;
  float x;
  float y;
  float z;
};

// Declare these variables at the top of your file
unsigned long lastDataReceived = 0;
unsigned long dataReceiveCount = 0;
unsigned long lastPing = 0;
unsigned long lastStatsPrint = 0;

// UART configuration
HardwareSerial FlightControllerSerial(0);  // Use UART2 for communication with flight controller
#define FC_BAUD_RATE 115200
#define RX2_PIN 3  // ESP32 pin GPIO16 connected to flight controller's TX
#define TX2_PIN 1  // ESP32 pin GPIO17 connected to flight controller's RX

// Updated OnDataRecv function to match esp_now_recv_cb_t
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  lastDataReceived = millis();
  dataReceiveCount++;
  lastPing = micros();
  
  // Cast the incoming data to our structure
  RandomData* data = (RandomData*)incomingData;
  
  // Print the received data in a format suitable for serial plotter
  Serial.printf("%d,%.1f,%.1f,%.1f\n", data->droneIndex, data->x, data->y, data->z);
  
  // Send data to flight controller via UART
  sendToFlightController(*data);
}

void sendToFlightController(const RandomData& data) {
  // Create a simple protocol to send data to the flight controller
  // Format: "R,<droneIndex>,<x>,<y>,<z>\n"
  FlightControllerSerial.printf("R,%d,%.2f,%.2f,%.2f\n", data.droneIndex, data.x, data.y, data.z);
}

void setup() {
  Serial.begin(115200);
  
  // Initialize UART communication with flight controller
  FlightControllerSerial.begin(FC_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Print MAC Address
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

  // Print column headers for serial plotter
  Serial.println("DroneIndex,X,Y,Z");
}

void loop() {
  unsigned long currentTime = millis();

  // Print stats every second
  if (currentTime - lastStatsPrint >= 1000) {
    float signalRate = (float)dataReceiveCount / ((currentTime - lastStatsPrint) / 1000.0);
    
    // Print stats to Serial
    Serial.printf("@Stats: Rate %.2f Hz, Total %lu\n", signalRate, dataReceiveCount);
    
    lastStatsPrint = currentTime;
    dataReceiveCount = 0; // Reset counter for next interval
  }

  // Check for any incoming data from the flight controller
  while (FlightControllerSerial.available()) {
    char c = FlightControllerSerial.read();
    // Process any incoming data from the flight controller if needed
    // For now, we'll just print it to the Serial monitor
    Serial.print(c);
  }

  delay(10);  // Small delay to prevent tight looping
}