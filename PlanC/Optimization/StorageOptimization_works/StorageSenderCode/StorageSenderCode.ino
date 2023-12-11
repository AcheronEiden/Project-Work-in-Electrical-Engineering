#include <ArduinoBLE.h>

// Define UUIDs for the BLE service and characteristics
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid1 = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid2 = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* timestampCharacteristicUuid = "19b10003-e8f2-537e-4f6c-d104768a1214";

// Create BLE service and characteristics instances
BLEService heartRateService(deviceServiceUuid);
BLEStringCharacteristic heartRateCharacteristic1(heartRateCharacteristicUuid1, BLERead | BLENotify, 50);
BLEStringCharacteristic heartRateCharacteristic2(heartRateCharacteristicUuid2, BLERead | BLENotify, 50);
BLEStringCharacteristic timestampCharacteristic(timestampCharacteristicUuid, BLERead | BLENotify, 20);

// Initialize variables for ECG lead values and sensor pins
const int heartRatePin1 = A0;  // Analog pin for ECG lead 1
const int heartRatePin2 = A1;  // Analog pin for ECG lead 2

const int bufferSize = 5; // Number of values to be buffered before updating
float values1[bufferSize];
float values2[bufferSize];
unsigned long timestampsArray[bufferSize];
int currentIndex = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
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
  BLE.setLocalName("Arduino Nano RP 2040 Connect (Dual Heart Rate Monitor)");
  BLE.setAdvertisedService(heartRateService);
  heartRateService.addCharacteristic(heartRateCharacteristic1);
  heartRateService.addCharacteristic(heartRateCharacteristic2);
  heartRateService.addCharacteristic(timestampCharacteristic);
  BLE.addService(heartRateService);

  // Initialize BLE characteristics with default or meaningful values
  // You can leave them uninitialized if not needed.
  // heartRateCharacteristic1.writeValue("Default1");
  // heartRateCharacteristic2.writeValue("Default2");
  // timestampCharacteristic.writeValue("DefaultTimestamp");

  BLE.advertise();

  // Print initialization information
  Serial.println("Nano RP2040 Connect (Dual Heart Rate Monitor)");
  Serial.println(" ");
}

void updateCharacteristics() {
  // Combine the values into a comma-separated string
  String valuesString1 = "";
  String valuesString2 = "";
  String timestampsString = "";

  for (int i = 0; i < bufferSize; ++i) {
    valuesString1 += String(values1[i], 2);
    valuesString2 += String(values2[i], 2);
    timestampsString += String(timestampsArray[i]);

    if (i < bufferSize - 1) {
      valuesString1 += ",";
      valuesString2 += ",";
      timestampsString += ",";
    }
  }

  // Update the BLE characteristics with the combined strings
  heartRateCharacteristic1.writeValue(valuesString1);
  heartRateCharacteristic2.writeValue(valuesString2);
  timestampCharacteristic.writeValue(timestampsString);

  Serial.println(valuesString1);
  Serial.println(valuesString2);
  Serial.println(timestampsString);
  // Clear the arrays and reset the index
  currentIndex = 0;
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

    // Continuously read analog values and update arrays
    while (central.connected()) {
      // Read the analog values from the heart rate monitors
      int sensor1Value = analogRead(heartRatePin1);
      int sensor2Value = analogRead(heartRatePin2);

      // Convert analog values to voltage
      float voltage1 = (sensor1Value / 1023.0) * 5.0;  // Assuming 5V reference voltage
      float voltage2 = (sensor2Value / 1023.0) * 5.0;  // Assuming 5V reference voltage

      // Store values in arrays
      values1[currentIndex] = voltage1;
      values2[currentIndex] = voltage2;
      timestampsArray[currentIndex] = millis()/1000;

      // Increment index
      currentIndex++;

      // Check if the buffer is full, then update characteristics
      if (currentIndex == bufferSize) {
        updateCharacteristics();
      }

      // Print voltage values to Serial Monitor
      //Serial.print("Voltage 1: ");
      //Serial.print(voltage1, 2);  // Display with 2 decimal places
      //Serial.print("V\tVoltage 2: ");
      //Serial.print(voltage2, 2);  // Display with 2 decimal places
      //Serial.println("V");

      // Delay for stability and to avoid data overload
      delay(10);
    }

    // If there are remaining values in the buffer, update characteristics
    if (currentIndex > 0) {
      updateCharacteristics();
    }

    Serial.println("* Disconnected from central device!");
  }
}