#include <ArduinoBLE.h>

// Define UUIDs for the BLE service and characteristics
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRate1CharacteristicUUID = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRate2CharacteristicUUID = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* timeCharacteristicUUID = "19b10003-e8f2-537e-4f6c-d104768a1214";

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
  BLE.setLocalName("ecgCentralDevice");
  BLE.advertise();

  // Print initialization information
  Serial.println("Arduino Nano 33 BLE Sense (Central Device for ECG readings)");
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
  float timeCurrent = 0.0;

  // Create instances of BLE characteristics for heart rate values
  BLECharacteristic heartRateCharacteristic1 = peripheral.characteristic(heartRate1CharacteristicUUID);
  BLECharacteristic heartRateCharacteristic2 = peripheral.characteristic(heartRate2CharacteristicUUID);
  BLECharacteristic timeCharacteristic = peripheral.characteristic(timeCharacteristicUUID);

  // Continuously read and print heart rate values while connected to the peripheral
  while (peripheral.connected()) {
    // Read heart rate values from BLE characteristics
    char valuesBuffer1[50], valuesBuffer2[50], timeBuffer[20];
    size_t size1 = heartRateCharacteristic1.readValue(valuesBuffer1, sizeof(valuesBuffer1));
    size_t size2 = heartRateCharacteristic2.readValue(valuesBuffer2, sizeof(valuesBuffer2));
    size_t size3 = timeCharacteristic.readValue(timeBuffer, sizeof(timeBuffer));

    // Convert the character arrays to Strings
    String valuesString1 = String(valuesBuffer1); // The string
    String valuesString2 = String(valuesBuffer2); // The string
    String timeString = String(timeBuffer);

    // Parse the comma-separated string values and convert to floats
    heartRateValue1 = getValueFromString(valuesString1, 0);
    heartRateValue2 = getValueFromString(valuesString2, 0);
    timeCurrent = getValueFromString(timeString, 0);

    // Print the formatted data to Serial Monitor
    //Serial.print("Time: ");
    //Serial.print(timeCurrent);
    //Serial.print(", Lead1 ECG: ");
    //Serial.print(heartRateValue1, 2);
    //Serial.print(", Lead2 ECG: ");
    //Serial.println(heartRateValue2, 2);
    Serial.print("Received String ASCII Values: ");
    for (int i = 0; i < timeString.length(); i++) {
      Serial.print(timeString[i]);
    }
    Serial.println();
  }

  // Print information when the peripheral device is disconnected
  Serial.println("- Peripheral device disconnected!");
}

// Function to extract a value from a comma-separated string
float getValueFromString(String data, int index) {
  int pos = 0;
  for (int i = 0; i < index; i++) {
    pos = data.indexOf(',', pos) + 1;
    if (pos == 0)
      return 0.0; // Return 0 if index not found
  }
  int nextPos = data.indexOf(',', pos);
  if (nextPos == -1)
    nextPos = data.length();
  return data.substring(pos, nextPos).toFloat();
}