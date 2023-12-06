#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid1 = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* heartRateCharacteristicUuid2 = "19b10002-e8f2-537e-4f6c-d104768a1214";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
  connectToPeripheral();
}

void connectToPeripheral() {
  BLEDevice peripheral;

  Serial.println("- Discovering peripheral device...");

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
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  float voltage1 = 0.0;
  float voltage2 = 0.0;
  char dataStr[50];  // Adjust the size as needed

  while (peripheral.connected()) {
    BLECharacteristic heartRateCharacteristic1 = peripheral.characteristic(heartRateCharacteristicUuid1);
    BLECharacteristic heartRateCharacteristic2 = peripheral.characteristic(heartRateCharacteristicUuid2);

    // Read voltage values
    heartRateCharacteristic1.readValue(&voltage1, 4);
    heartRateCharacteristic2.readValue(&voltage2, 4);

    // Create a string with millis, voltage1, and voltage2
    sprintf(dataStr, "Millis: %lu, Voltage 1: %.2fV, Voltage 2: %.2fV", millis(), voltage1, voltage2);

    // Print the string to Serial Monitor
    Serial.println(dataStr);

    delay(1); // Adjust delay as needed
  }

  Serial.println("- Peripheral device disconnected!");
}