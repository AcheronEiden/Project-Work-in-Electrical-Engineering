/*
  Peripheral device for Dual Heart Rate Monitor
  The Arduino used is Nano RP2040 Connect
  
  This script is designed for an Arduino to function as the Peripheral device for a dual heart rate monitor,
  using Bluetooth Low Energy (BLE) for communication. It reads analog values from two heart rate sensors,
  converts them to voltage, and updates the BLE characteristics with the voltage values. Finally it sends
  the data via bluetooth using BLE for a central device to read the data.

  Hardware Setup:
  - Connect two heart rate sensors to analog pins A0 and A1.
  - Ensure a 5V reference for the arduino.

  Bluetooth Service and Characteristics:
  - The Arduino advertises a custom BLE service with two float characteristics
      (heartRateCharacteristic1 & heartRateCharacteristic2).
  - Voltage values are advertised for a central device to read the values.

  Note:
  - This sketch uses the ArduinoBLE library for Bluetooth Low Energy (BLE) functionality.
  - Ensure that the Arduino Nano RP2040 Connect is properly powered,
      configured to function as a peripheral device and paired with a central device.
  - Adjust delay value for obetter readability and real-time data handling.
      Make sure that th delay for this code (the central device) and the connected
      periperhal device is the same as to not introduce any unexpected issues.

  Author: Alhassan Jawad, Andreas Hertzberg & Niklas Gernandt
  Date: 10/12-2023
  For any questions, contact: Alhassan.Jawad.9989@student.uu.se or https://github.com/AcheronEiden
*/
//----------------------------------------------------------------------------------------------------

#include <ArduinoBLE.h>
// Constants & initializations
const int analogPin1 = A0; // Pin for the first heart monitor (lead1)
const int analogPin2 = A1; // Pin for the second heart monitor (lead2)

unsigned long previousTime = 0; // store last time values were read
unsigned long interval = 500; // interval for each reading, given in milliseconds
unsigned long timeCounter = 0;

const char* deviceName = "ECGPeripheral";
    // Custom 128-bits (32characters*4bitsPerCharacter) UUIDs
const char* deviceServiceUUID = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRate1CharacteristicUUID = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRate2CharacteristicUUID = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* timeCharacteristicUUID = "19b10003-e8f2-537e-4f6c-d104768a1214";

BLEService ecgService(deviceServiceUUID);
BLEFloatCharacteristic lead1Characteristic(heartRate1CharacteristicUUID, BLERead | BLENotify); // Read OR Notify
BLEFloatCharacteristic lead2Characteristic(heartRate2CharacteristicUUID, BLERead | BLENotify); // Read OR Notify
BLEFloatCharacteristic timeCharacteristic(timeCharacteristicUUID, BLERead | BLENotify); // Read OR Notify

void setup() {
  // Initialize serial communication of 9600 baud rate,
  // meaning 9600 signal-changes/second
  Serial.begin(9600);

  while(!Serial); // Wait in this loop until communication is established
  
  // If initializing connection failed -> print message and wait
  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth Low Energy Module Failed!");
    //while(1)
  }
  // If initializing connection succeed -> set name, service and make advertisement
  BLE.setLocalName(deviceName);
  BLE.setAdvertisedService(ecgService);
  ecgService.addCharacteristic(lead1Characteristic);
  ecgService.addCharacteristic(lead2Characteristic);
  ecgService.addCharacteristic(timeCharacteristic);  
  BLE.addService(ecgService);
  BLE.advertise();

  Serial.println("ECG Peripheral Connected Successfully");
}

/*
For timing I will use the millis() function that returns the number of ms
passed since the program started. The Data Type is unsigned long.
*/

void loop() {
  unsigned long currentTime = millis(); // Grab specifically the current time
  BLEDevice central = BLE.central();

  /* Instead of delay() at the end of the loop code, I will use an if-statement
  to compare the current taken millis() with the previous taken one and see if
  a set amount of constant time has went since the last voltage reading.
  This is done as delay() will just pause the code and may lead to ECG peaks
  not being read and shown.
  */
  if (central) {
    Serial.print("Connected to central device: ");
    // Show address of the central device connected with this peripheral device
    Serial.println(central.address());
    while (central.connected()) {
      if (currentTime - previousTime >= interval) {
        previousTime = currentTime;
        // Read analog values
        int sensorValue1 = analogRead(analogPin1);
        int sensorValue2 = analogRead(analogPin2);

        // Convert analog values to voltage assuming a 5V reference voltage
        // Assumed 5V reference voltage as that is what the arduino is powered on
        // Regarding the heart monitors, they have a power source of 3.3V
        float lead1ECG = (sensorValue1 / 1023.0) * 5.0;
        float lead2ECG = (sensorValue2 / 1023.0) * 5.0;

        /* The following 9 lines of code are for
        sending the data as byte arrays instead of floats.
        byte lead1Bytes[sizeof(float)];
        byte lead2Bytes[sizeof(float)];
        byte timeTaken[sizeof(float)];
        memcpy(lead1Bytes, &lead1ECG, sizeof(float));
        memcpy(lead2Bytes, &lead2ECG, sizeof(float));
        memcpy(timeTaken, &currentTime, sizeof(float));
        lead1Characteristic.writeValue(lead1Bytes, sizeof(float));
        lead2Characteristic.writeValue(lead2Bytes, sizeof(float));
        timeCharacteristic.writeValue(timeTaken, sizeof(float));
        */

        // Update BLE with characteristics
        lead1Characteristic.writeValue(lead1ECG);
        lead2Characteristic.writeValue(lead2ECG);
        timeCharacteristic.writeValue(currentTime);

        // Print statistics to the serial monitor
        // Not printing any text for the values so that I can use these values
        // to plot in Python at a later date
        Serial.print(timeCounter);
        Serial.print(", ");
        Serial.print(lead1ECG, 2);
        Serial.print(", ");
        Serial.println(lead2ECG, 2);

        // Increment timeCounter to move along the timer
        timeCounter++;
      }
    }
    Serial.println("Disconnected from central device!");
  }
}