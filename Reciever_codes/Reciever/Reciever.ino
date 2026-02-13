#include <SPI.h>
#include <LoRa.h>

#define SS   5
#define RST  27
#define DIO0 26

void setup() {
  Serial.begin(115200);

  SPI.begin(18, 19, 23, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("RX failed");
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);

  Serial.println("RX Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received: ");
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}

