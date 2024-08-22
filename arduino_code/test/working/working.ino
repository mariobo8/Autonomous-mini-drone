#include <HardwareSerial.h>
#include "sbus.h"

// Initialize the SBUS transmission object
bfs::SbusTx sbusTx(&Serial1, 10, 9, false);  // Using Serial1, TX=16, RX=17, not inverted signal

void setup() {
  // Start the SBUS transmission
  sbusTx.Begin();

  // Configure the Serial port for debugging
  Serial.begin(115200);
}

void loop() {

  // Create data for 16 channels, set a constant value (1024) for all channels
  bfs::SbusData data;
  data.ch[0]=993;
  data.ch[1]=993;
  data.ch[3]=993;
  data.ch[2]=400;
  data.ch[4]=1200;

  for (int i = 5; i < bfs::SbusData::NUM_CH; i++) {
    data.ch[i] = 300;  // Mid-point for all channels
  }
  

  data.lost_frame = false;
  data.failsafe = false;
  data.ch17 = false;
  data.ch18 = false;

  // Set the data to the SBUS transmission object
  sbusTx.data(data);

  // Send the SBUS data
  sbusTx.Write();

  // Print the data for debugging
  for (int i = 0; i < bfs::SbusData::NUM_CH; i++) {
    Serial.print("Channel ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(data.ch[i]);
  }

  // Delay to match SBUS frame rate (14ms for ~72Hz)
  delay(14);
}
