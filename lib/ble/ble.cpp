#include <secrets.h>

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>

#define SERVICE_UUID "14839ac4-7d7e-415c-9a42-167340cf2339"

// magic stuff for the Viatom GATT service
#define BLE_WRITE_UUID_PREFIX "8b00ace7"

// notify handles will have a UUID that begins with this
#define BLE_NOTIFY_UUID_PREFIX "00002902"

uint8_t write_bytes_data[] = {0xaa, 0x17, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x1b};
size_t write_bytes_length = sizeof(write_bytes_data) / sizeof(write_bytes_data[0]);

// these are the byte values that we need to write to subscribe/unsubscribe for notifications
uint8_t subscribe_bytes_data[] = {0x01, 0x00};
size_t subscribe_bytes_length = sizeof(subscribe_bytes_data) / sizeof(subscribe_bytes_data[0]);

//uint8_t unsubscribe_bytes_data[] = {0x00, 0x00};
//size_t unsubscribe_bytes_length = sizeof(subscribe_bytes_data) / sizeof(subscribe_bytes_data[0]);


uint8_t write_bytes_data_vibration_general[] = {0xaa, 0x16, 0xe9, 0x00, 0x00, 0x11, 0x00, 0x7b, 0x22, 0x53, 0x65, 0x74, 0x4d, 0x6f, 0x74, 0x6f, 0x72, 0x22, 0x3a, 0x22};
size_t write_bytes_length_vibration_general = sizeof(write_bytes_data_vibration_general) / sizeof(write_bytes_data_vibration_general[0]);

uint8_t write_bytes_data_vibration_very_weak[] = {0x32, 0x37, 0x22, 0x7d, 0xb2};
size_t write_bytes_length_vibration_very_weak = sizeof(write_bytes_data_vibration_very_weak) / sizeof(write_bytes_data_vibration_very_weak[0]);

uint8_t write_bytes_data_vibration_weak[] = {0x34, 0x30, 0x22, 0x7d, 0xd0};
size_t write_bytes_length_vibration_weak = sizeof(write_bytes_data_vibration_weak) / sizeof(write_bytes_data_vibration_weak[0]);

uint8_t write_bytes_data_vibration_medium[] = {0x36, 0x30, 0x22, 0x7d, 0xfc};
size_t write_bytes_length_vibration_medium = sizeof(write_bytes_data_vibration_medium) / sizeof(write_bytes_data_vibration_medium[0]);

uint8_t write_bytes_data_vibration_strong[] = {0x38, 0x30, 0x22, 0x7d, 0x38};
size_t write_bytes_length_vibration_strong = sizeof(write_bytes_data_vibration_strong) / sizeof(write_bytes_data_vibration_strong[0]);

uint8_t write_bytes_data_vibration_very_strong[] = {0x31, 0x30, 0x30, 0x22, 0x7d, 0x68};
size_t write_bytes_length_vibration_very_strong = sizeof(write_bytes_data_vibration_very_strong) / sizeof(write_bytes_data_vibration_very_strong[0]);

// {"SetTIME":"2
uint8_t write_bytes_data_time_general[] = {0xaa, 0x16, 0xe9, 0x00, 0x00, 0x21, 0x00, 0x7b, 0x22, 0x53, 0x65, 0x74, 0x54, 0x49, 0x4d, 0x45, 0x22, 0x3a, 0x22, 0x32};
size_t write_bytes_length_time_general = sizeof(write_bytes_data_time_general) / sizeof(write_bytes_data_time_general[0]);

// Convert the MAC address string to a BLEAddress object
BLEAddress macAddress(O2RING_MAC);

// Use BLE_ADDR_TYPE_RANDOM for random addresses
esp_ble_addr_type_t addrType = BLE_ADDR_TYPE_RANDOM;

uint16_t subscribeHandle;
uint16_t writeHandle;

BLERemoteDescriptor* pSubscribeDescriptor;
BLERemoteCharacteristic* pWriteCharacteristic;

BLEClient* pClient;
BLERemoteService* pRemoteService;

String influxPayload = "";

bool device_is_worn = false;

String get_influx_payload() {
    return influxPayload;
}

void set_influx_payload(String payload){
    influxPayload = payload;
}

bool is_device_worn() {
  return device_is_worn;
}

// Callback for reading notifications
void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* data, size_t length, bool isNotify) {
  if (isNotify && length > 1) {
    Serial.println("Notification received!");
    // Process received data
    for (size_t i = 0; i < length; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    if (length < 20) {
      device_is_worn = false;
    } else {
      if (data[18] == 0) {
        device_is_worn = false;
        int battery = data[14];
        Serial.print("Device is not being worn!\tBattery: ");
        Serial.print(battery);
        Serial.print("%");
        Serial.println();
      } else if (data[17] == 0 && data[8] == 0) {
        device_is_worn = true;
        int battery = data[14];
        Serial.print("Device is calibrating...\tBattery: ");
        Serial.print(battery);
        Serial.print("%");
        Serial.println();
      } else {
        device_is_worn = true;
        int spO2 = data[7];
        int hearRate = data[8];
        int battery = data[14];
        int movement = data[16];
        int pi = data[17];
        Serial.println("SpO2: " + String(spO2) + "%\tHR: " + String(hearRate) + " bpm\tPI: " + String(pi) + "\tMovement: " + String(movement) + "\tBattery: " + String(battery) + "%");

        influxPayload = "o2ring spo2=" + String(spO2) + ",hr=" + String(hearRate) + ",pi=" + String(pi) + ",movement=" + String(movement) + ",battery=" + String(battery);
      }
    }
  }
}


void discover_device() {
    // Discover services and characteristics after connecting
    pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService) {
      Serial.println("Service found");

      // Get all characteristics of the service
      std::map<std::string, BLERemoteCharacteristic*>* characteristics = pRemoteService->getCharacteristics();
      // Serial.println("Characteristics found:");

      // Iterate over all characteristics and print their UUIDs
      for (const auto& kv : *characteristics) {
        BLERemoteCharacteristic* pCharacteristic = kv.second;

        String characteristicUUID = pCharacteristic->getUUID().toString().c_str();

        // Serial.print("Characteristic: ");
        // Serial.println(characteristicUUID);

        if (pCharacteristic->canNotify()) {
          pCharacteristic->registerForNotify(notifyCallback);
        }

        if (characteristicUUID.startsWith(BLE_WRITE_UUID_PREFIX)) {
          // Save the handle of the characteristic
          writeHandle = pCharacteristic->getHandle();
          pWriteCharacteristic = pCharacteristic;
          Serial.print("writeHandle: ");
          Serial.println(writeHandle);
        }

        // Get descriptors for this characteristic
        std::map<std::string, BLERemoteDescriptor*>* descriptors = pCharacteristic->getDescriptors();        
        if (descriptors->empty()) {
          // Serial.println("No descriptors found for this characteristic.");
        } else {
          // Serial.println("Descriptors found:");
          for (const auto& kvDescriptor : *descriptors) {
            BLERemoteDescriptor* pDescriptor = kvDescriptor.second;

            String descriptorUUID = pDescriptor->getUUID().toString().c_str();
            
            // Serial.print("Descriptor UUID: ");
            // Serial.println(descriptorUUID);

            if (descriptorUUID.startsWith(BLE_NOTIFY_UUID_PREFIX)) {
              // Save the handle of the descriptor
              subscribeHandle = pDescriptor->getHandle();
              pSubscribeDescriptor = pDescriptor;
              Serial.print("subscribeHandle: ");
              Serial.println(subscribeHandle);
            }
          }
        }
      }
    }
}

void set_vibration_weak() {
    pWriteCharacteristic->writeValue(write_bytes_data_vibration_general, write_bytes_length_vibration_general, true);
    pWriteCharacteristic->writeValue(write_bytes_data_vibration_weak, write_bytes_length_vibration_weak, true);
}

void stringToHexArray(const String &str, uint8_t *hexArray, size_t maxLen) {
    size_t len = str.length();
    if (len > maxLen) {
        len = maxLen;
    }
    
    for (size_t i = 0; i < len; i++) {
        hexArray[i] = static_cast<uint8_t>(str[i]);
    }
}

// CRC-8 calculation with polynomial 0x07
uint8_t crc8(uint8_t* data, size_t length) {
  uint8_t crc = 0x00;  // Initial value

  for (size_t i = 0; i < length; i++) {
    crc ^= data[i];  // XOR the byte with the current CRC value
    for (byte bit = 0; bit < 8; bit++) {
      // Check if the highest bit is set (MSB)
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;  // Apply the polynomial (0x07)
      } else {
        crc <<= 1;  // Just shift left if no need to apply polynomial
      }
      crc &= 0xFF;  // Ensure the CRC remains an 8-bit value
    }
  }
  return crc;
}

void set_time(String time) {
    time.remove(0, 1);  // Remove 1 character at index 0 because the "2" of the year is provided with the first write command
    String time_input = time + "\"}";
    Serial.println(time_input);

    uint8_t time_hexArray[time_input.length()];
    stringToHexArray(time_input, time_hexArray, sizeof(time_hexArray));
    size_t time_hexArray_length = sizeof(time_hexArray) / sizeof(time_hexArray[0]);

    // Combine data1 parts and data2 parts into full data arrays
    size_t write_bytes_combined_size = sizeof(write_bytes_data_time_general) + sizeof(time_hexArray);
    uint8_t write_bytes_combined[write_bytes_combined_size];
    
    // Copy data1_part1 into the data1 array
    memcpy(write_bytes_combined, write_bytes_data_time_general, sizeof(write_bytes_data_time_general));
    
    // Copy data1_part2 into the data1 array, starting after data1_part1
    memcpy(write_bytes_combined + sizeof(write_bytes_data_time_general), time_hexArray, sizeof(time_hexArray));

    uint8_t checksum = crc8(write_bytes_combined, sizeof(write_bytes_combined));
    uint8_t checksum_array[] = { checksum };
    size_t checksum_array_length = sizeof(checksum_array) / sizeof(checksum_array[0]);

    pWriteCharacteristic->writeValue(write_bytes_data_time_general, write_bytes_length_time_general, true);
    pWriteCharacteristic->writeValue(time_hexArray, time_hexArray_length, true);
    pWriteCharacteristic->writeValue(checksum_array, checksum_array_length, true);
}

bool connect_device() {
    // Connect to the device using its MAC address
    Serial.print("Connecting to: ");
    Serial.println(macAddress.toString().c_str());

    // Try connecting
    bool connected = pClient->connect(macAddress, addrType);

    if (connected) {
        Serial.println("Connected to device");
        discover_device();
    } else {
        Serial.println("Failed to connect to device");
    }

    return connected;
}

void request_data() {
    pSubscribeDescriptor->writeValue(subscribe_bytes_data, subscribe_bytes_length, true);
    pWriteCharacteristic->writeValue(write_bytes_data, write_bytes_length, true);
    Serial.println("\nSent bytes to ring.");
}

void init_ble() {
    // Initialize BLE
    BLEDevice::init("");

    // Create the BLE client
    pClient = BLEDevice::createClient();
    Serial.println("Created client");
}