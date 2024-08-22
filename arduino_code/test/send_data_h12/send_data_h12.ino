#include <HardwareSerial.h>

#define UART1_TXD 9
#define UART1_RXD 10
#define H12_BAUD_RATE 9600
#define LED_PIN 2 

HardwareSerial h12Serial(1);  // Use UART1 instead of UART0

void setup() {
  Serial.begin(115200);
  h12Serial.begin(H12_BAUD_RATE, SERIAL_8N1, UART1_RXD, UART1_TXD);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("H12 Loopback Test");
}

void loop() {
  // Send a message
  String message = "Loopback Test";
  h12Serial.println(message);
  Serial.println("Sent: " + message);
  
  // Blink LED to indicate transmission
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  
  // Check for incoming data
  if (h12Serial.available()) {
    String receivedMessage = h12Serial.readStringUntil('\n');
    Serial.println("Received: " + receivedMessage);
  }
  
  delay(1700);  // Total delay of about 2 seconds per cycle
}