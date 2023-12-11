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
    heartRateCharacteristic1.readValue(&heartRateValue1, sizeof(float));
    heartRateCharacteristic2.readValue(&heartRateValue2, sizeof(float));
    timeCharacteristic.readValue(&timeCurrent, sizeof(float));

    // Print the formatted data to Serial Monitor
    // Not printing any text for the values so that I can use these values
    // to plot in Python at a later date, and store the data in a csv file for future plotting
    //Serial.print("Time: ")
    Serial.print(timeCurrent);
    //Serial.print(", Lead1 ECG: ")
    Serial.print(", ");
    Serial.print(heartRateValue1, 2);
    //Serial.print(", Lead2 ECG: ")
    Serial.print(", ");
    Serial.println(heartRateValue2, 2);
  }

  // Print information when the peripheral device is disconnected
  Serial.println("- Peripheral device disconnected!");
}