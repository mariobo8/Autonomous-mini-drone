#include <HardwareSerial.h>
#include "sbus.h"

// Initialize the SBUS transmission object
bfs::SbusTx sbusTx(&Serial1, 10, 9, false);  // Using Serial1, TX=16, RX=17, not inverted signal

// Create data for 16 channels, set a constant value (1024) for all channels
bfs::SbusData data;

unsigned long lastSbusSend = micros();
float sbusFrequency = 50.0;

void setup() {
  // Start the SBUS transmission
  sbusTx.Begin();


  data.failsafe = false;
  data.ch17 = true;
  data.ch18 = true;
  data.lost_frame = false;
  for (int i = 500; i > 172; i--) {
    for (int j = 0; j < 16; j++) {
      data.ch[j] = i;
    }
    Serial.println(i);
    sbusTx.data(data);
    sbusTx.Write();
  }

  // Configure the Serial port for debugging
  Serial.begin(115200);
  lastSbusSend = micros();

}

void loop() {


  data.ch[0] = 992;
  data.ch[1] = 992;
  data.ch[2] = 992;
  data.ch[3] = 992;
  data.ch[4] = 1800;
  //for (int i = 5; i < bfs::SbusData::NUM_CH; i++) {
  //  data.ch[i] = 300;  // Mid-point for all channels
  //}
  

//  data.lost_frame = false;
 // data.failsafe = false;
//  data.ch17 = false;
 // data.ch18 = false;

  if (micros() - lastSbusSend > 1e6 / sbusFrequency) {
    lastSbusSend = micros();
    // Serial.printf("PWM x: %d, y: %d, z: %d, yaw: %d\nPos x: %f, y: %f, z: %f, yaw: %f\n", xPWM, yPWM, zPWM, yawPWM, xVel, yVel, zPos, yawPos);
    // Serial.printf("Setpoint x: %f, y: %f, z: %f\n", xVelSetpoint, yVelSetpoint, zVelSetpoint);
    // Serial.printf("Pos x: %f, y: %f, z: %f\n", xVel, yVel, zPos);
    //Serial.printf("Output x: %f, y: %f, z: %f\n", xVelOutput, yVelOutput, zVelOutput);

    sbusTx.data(data);
    sbusTx.Write();
  }


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
