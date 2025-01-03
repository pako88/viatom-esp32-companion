#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>

String get_influx_payload();
void set_influx_payload(String payload);
bool is_device_worn();
void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* data, size_t length, bool isNotify);
void discover_device();
bool connect_device();
void request_data();
void set_vibration_weak();
void stringToHexArray(const String &str, uint8_t *hexArray, size_t maxLen);
uint8_t crc8(uint8_t* data, size_t length);
void set_time(String time);
void init_ble();

#endif