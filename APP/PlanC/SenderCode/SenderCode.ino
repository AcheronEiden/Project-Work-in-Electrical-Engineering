/*
  Sender device for Dual Heart Rate Monitor
  The Arduino used is Nano 2060 Connect
  
  This script is designed for an Arduino to function as a dual heart rate monitor,
  using Bluetooth Low Energy (BLE) for communication. It reads analog values from two heart rate sensors,
  converts them to voltage, and updates the BLE characteristics with the voltage values. Finally it sends
  the data via bluetooth using BLE to a receivers device.

  Hardware Setup:
  - Connect two heart rate sensors to analog pins A0 and A1.
  - Ensure a 5V reference voltage for the sensors.

  Bluetooth Service and Characteristics:
  - The Arduino advertises a custom BLE service with two float characteristics (ECG lead 1 and ECG lead 2).
  - Voltage values are sent to the connected receiver device in real-time.

  Note:
  - This sketch uses the ArduinoBLE library for Bluetooth Low Energy functionality.
  - Ensure that the Arduino Nano 2060 Connect is properly powered and paired with a receiver device.

  Author: Alhassan Jawad, Andreas Hertzberg & Niklas Gernandt
  Date: 5/12-2023
  For any questions contact: Alhassan.Jawad.9989@student.uu.se or https://github.com/AcheronEiden
*/

#include <ArduinoBLE.h>

// Define UUIDs for the BLE service and characteristics
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid1 = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid2 = "19b10002-e8f2-537e-4f6c-d104768a1214";

// Create BLE service and characteristics instances
BLEService heartRateService(deviceServiceUuid);
BLEFloatCharacteristic heartRateCharacteristic1(heartRateCharacteristicUuid1, BLERead | BLENotify);
BLEFloatCharacteristic heartRateCharacteristic2(heartRateCharacteristicUuid2, BLERead | BLENotify);

// Initialize variables for ECG lead values and sensor pins
float lead1 = 0.0;
float lead2 = 0.0;
const int heartRatePin1 = A0;  // Analog pin for ECG lead 1
const int heartRatePin2 = A1;  // Analog pin for ECG lead 2

void setup() {
  // Initialize serial communication
  Serial.begin(9600); // baud rate of 9600 was found to be enough
  pinMode(heartRatePin1, INPUT);
  pinMode(heartRatePin2, INPUT);
  
  // Wait for serial connection to be established
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // Set the local name and advertise the BLE service
  BLE.setLocalName("Arduino Nano 33 BLE (Dual Heart Rate Monitor)");
  BLE.setAdvertisedService(heartRateService);
  heartRateService.addCharacteristic(heartRateCharacteristic1);
  heartRateService.addCharacteristic(heartRateCharacteristic2);
  BLE.addService(heartRateService);

  // Initialize and set initial values for the BLE characteristics
  heartRateCharacteristic1.writeValue(lead1);
  heartRateCharacteristic2.writeValue(lead2);
  
  BLE.advertise();

  // Print initialization information
  Serial.println("Nano 33 BLE (Dual Heart Rate Monitor)");
  Serial.println(" ");
}

void loop() {
  // Wait for a central device to connect
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    // Continuously read analog values and update BLE characteristics
    while (central.connected()) {
      // Read the analog values from the heart rate monitors
      int sensor1Value = analogRead(heartRatePin1);
      int sensor2Value = analogRead(heartRatePin2);

      // Convert analog values to voltage
      float voltage1 = (sensor1Value / 1023.0) * 5.0;  // Assuming 5V reference voltage
      float voltage2 = (sensor2Value / 1023.0) * 5.0;  // Assuming 5V reference voltage

      // Update the BLE characteristics with the voltage values
      heartRateCharacteristic1.writeValue(voltage1);
      heartRateCharacteristic2.writeValue(voltage2);

      // Print voltage values to Serial Monitor
      Serial.print("Voltage 1: ");
      Serial.print(voltage1, 2);  // Display with 2 decimal places
      Serial.print("V\tVoltage 2: ");
      Serial.print(voltage2, 2);  // Display with 2 decimal places
      Serial.println("V");

      // Delay for stability and to avoid data overload
      delay(10);
    }

    Serial.println("* Disconnected from central device!");
  }
}