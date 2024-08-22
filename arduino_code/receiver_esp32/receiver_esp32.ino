#include <Arduino.h>

#define SBUS_PIN 32
#define SBUS_BAUD 100000
#define SBUS_UPDATE_RATE 50 // 50Hz update rate

const int SBUS_PACKET_LENGTH = 25;
uint8_t sbusPacket[SBUS_PACKET_LENGTH];
uint16_t channels[16];
unsigned long lastSbusTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize UART for SBUS with inverted signal
  Serial1.begin(SBUS_BAUD, SERIAL_8E2, 16, SBUS_PIN, true); // true enables inversion
  
  // Initialize channels to mid-point
  for (int i = 0; i < 16; i++) {
    channels[i] = 992;
  }

  Serial.println("Inverted SBUS Transmitter Started");
}

void createSbusPacket() {
  sbusPacket[0] = 0x0F; // Start byte
  sbusPacket[24] = 0x00; // End byte

  // Pack 16 11-bit channels into 22 bytes
  for (int i = 0; i < 176; i++) {
    int bitIndex = i + 11 * (i / 11);
    int byteIndex = 1 + bitIndex / 8;
    int bitOffset = bitIndex % 8;
    
    uint16_t channelValue = channels[i / 11];
    bool bitValue = (channelValue & (1 << (i % 11))) != 0;
    
    if (bitValue) {
      sbusPacket[byteIndex] |= (1 << bitOffset);
    } else {
      sbusPacket[byteIndex] &= ~(1 << bitOffset);
    }
  }

  // Set flags (last 2 bits of last channel byte)
  sbusPacket[23] &= 0b11111100;
  // Uncomment these lines if you need to set specific flags
  // sbusPacket[23] |= 0b00000001; // Set if channel 17 is on
  // sbusPacket[23] |= 0b00000010; // Set if channel 18 is on
  // sbusPacket[23] |= 0b00001000; // Set if failsafe is active
  // sbusPacket[23] |= 0b00000100; // Set if frame lost
}

void updateChannels() {
  static int value = 172;
  static bool increasing = true;

  if (increasing) {
    value += 10;
    if (value >= 1811) increasing = false;
  } else {
    value -= 10;
    if (value <= 172) increasing = true;
  }

  for (int i = 0; i < 16; i++) {
    channels[i] = value;
  }

  Serial.printf("Channel Value: %d\n", value);
}

void loop() {
  if (millis() - lastSbusTime >= (1000 / SBUS_UPDATE_RATE)) {
    updateChannels();
    createSbusPacket();
    Serial1.write(sbusPacket, SBUS_PACKET_LENGTH);
    lastSbusTime = millis();
  }
}