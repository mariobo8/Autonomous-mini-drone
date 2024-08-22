#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32-Quadcopter";
const char* password = "123456789";

WiFiUDP udp;
unsigned int localUdpPort = 4210;  // local port to listen on

// Variables to store control values
int throttle = 0;
int pitch = 0;
int roll = 0;
int yaw = 0;

void setup() {
  Serial.begin(115200);
  
  // Set up Access Point
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", IP.toString().c_str(), localUdpPort);

  // Print column headers for Serial Plotter
  Serial.println("Throttle,Pitch,Roll,Yaw");
}

void loop() {
  char incomingPacket[255];  // buffer for incoming packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    
    // Parse control data
    sscanf(incomingPacket, "T:%d,P:%d,R:%d,Y:%d", &throttle, &pitch, &roll, &yaw);
  }
  
  // Plot the values (Serial Plotter format)
  Serial.printf("%d,%d,%d,%d\n", throttle, pitch, roll, yaw);
  
  // Small delay to control the plot update rate
  delay(50);  // Update at 20Hz
}