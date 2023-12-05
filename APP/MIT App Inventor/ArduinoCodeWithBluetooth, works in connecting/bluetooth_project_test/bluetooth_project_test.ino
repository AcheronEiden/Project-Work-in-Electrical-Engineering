#include <ArduinoBLE.h>

BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Define a custom BLE service
BLEFloatCharacteristic customCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite); // Define a custom BLE characteristic

void setup() {
  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  BLE.setLocalName("ArduinoNanoRP2040"); // Set the local name for the BLE peripheral
  BLE.setAdvertisedService(customService); // Advertise the custom service
  customService.addCharacteristic(customCharacteristic); // Add the custom characteristic to the service
  BLE.addService(customService); // Add the service
  BLE.advertise(); // Start advertising

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central(); // Check if a central device has connected
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.print("Connected to central device: ");
    Serial.println(central.address());

    while (central.connected()) {
      // Read data or sensor values
      int sensorValue = analogRead(A0);

      // Send the data to the central device (MIT App Inventor)
      customCharacteristic.writeValue(sensorValue);
      Serial.println(sensorValue);

      delay(10); // Adjust delay as needed
    }

    Serial.println("Disconnected from central device.");
  }
}