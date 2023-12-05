/*
  Receiver device for Heart Rate Monitor
  The Arduino used is Nano 33 BLE

  This script is designed for an arduino to get date via bluetooth and print the data in
  a serial monitor. for a heart rate monitor using Bluetooth Low Energy (BLE).
  The device connects to a sender heart rate monitor/arduino device, reads the sent
  heart rate data, and prints the formatted data
  (current time given in ms, value for lead1, value for lead2) to the Serial Monitor.

  Bluetooth Service and Characteristics:
  - The receiver device advertises and connects to a peripheral device with specific service UUID.
  - Two float characteristics represent heart rate values from sensors 1  (lead1) and 2 (lead2) respectively.

  Note:
  - This sketch uses the ArduinoBLE library for Bluetooth Low Energy functionality.
  - Ensure the Arduino Nano 33 BLE is properly powered and configured to function as a receiver device.
  - Adjust delay values for better readability and real-time data handling.

  Author: Alhassan Jawad, Andreas Hertzberg & Niklas Gernandt
  Date: 5/12-2023
  For any questions contact: Alhassan.Jawad.9989@student.uu.se or https://github.com/AcheronEiden
*/

#include <ArduinoBLE.h>

// Define UUIDs for the BLE service and characteristics
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid1 = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid2 = "19b10002-e8f2-537e-4f6c-d104768a1214";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // Set local name and advertise the BLE service
  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();

  // Print initialization information
  Serial.println("Arduino Nano 33 BLE Sense (Receiver Device)");
  Serial.println(" ");
}

void loop() {
  // Discover and connect to a peripheral device
  connectToPeripheral();
}

void connectToPeripheral() {
  BLEDevice peripheral;

  // Print information during peripheral discovery
  Serial.println("- Discovering peripheral device...");

  // Scan for peripheral devices advertising the specified service UUID
  do {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);

  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");

    // Stop scanning and proceed to control the connected peripheral
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  // Print information during connection establishment
  Serial.println("- Connecting to peripheral device...");

  // Connect to the discovered peripheral device
  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  // Print information during peripheral attribute discovery
  Serial.println("- Discovering peripheral device attributes...");

  // Discover attributes (services and characteristics) of the connected peripheral
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  // Initialize variables for heart rate values
  float heartRateValue1 = 0.0;
  float heartRateValue2 = 0.0;

  // Create instances of BLE characteristics for heart rate values
  BLECharacteristic heartRateCharacteristic1 = peripheral.characteristic(heartRateCharacteristicUuid1);
  BLECharacteristic heartRateCharacteristic2 = peripheral.characteristic(heartRateCharacteristicUuid2);

  // Continuously read and print heart rate values while connected to the peripheral
  while (peripheral.connected()) {
    // Read heart rate values from BLE characteristics
    heartRateCharacteristic1.readValue(&heartRateValue1, 4);
    heartRateCharacteristic2.readValue(&heartRateValue2, 4);

    // Get the current millis (ms) value for timestamping
    unsigned long currentMillis = millis();

    // Print the formatted data to Serial Monitor
    Serial.print(currentMillis);
    Serial.print(", ");
    Serial.print(heartRateValue1, 2);
    Serial.print(", ");
    Serial.println(heartRateValue2, 2);

    // Introduce a delay for better readability and data handling (adjust as needed)
    delay(10);
  }

  // Print information when the peripheral device is disconnected
  Serial.println("- Peripheral device disconnected!");
}