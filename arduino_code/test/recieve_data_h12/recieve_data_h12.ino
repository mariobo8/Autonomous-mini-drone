#include <HardwareSerial.h>

#define UART0_TXD 1
#define UART0_RXD 3
#define H12_BAUD_RATE 9600
#define LED_PIN 2 

HardwareSerial h12Serial(0);

void setup() {
  Serial.begin(115200);
  h12Serial.begin(H12_BAUD_RATE, SERIAL_8N1, UART0_RXD, UART0_TXD);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("H12 Loopback Test");
}

void loop() {
  
  // Check if data is available to read
  if (h12Serial.available()) {
    String receivedData = h12Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(receivedData);
  } else {
    Serial.println("No data received. Check your connections.");
  }
  
  delay(2000);  // Wait 2 seconds before the next test
}