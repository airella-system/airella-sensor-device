#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "communication/bluetooth/chunker/BluetoothChunkReceiver.h"

#include "BluetoothHandler.h"
#include "communication/common/Internet.h"

#define SERVICE_UUID "f1eb6601-af50-4cf3-9f6e-4e1c6fb8e88c"

// Bluetooth password - something we must re-consider
#define DEVICE_PASSWORD_CUUID "126b5b11-6590-4229-8026-ba30ad032133"

// WIFI or GSM
#define INTERNET_CONNECTION_TYPE_CUUID "7d9059a5-f426-4f2f-9050-3c88036beb1b"

// WiFi SSID
#define SSID_CUUID "45bee5c6-8043-4c6e-b497-fad68a340b70"

// WiFi password
#define WIFI_PASWORD_CUUID "0851dd07-b59f-40c6-8114-357c7dff694c"

// Registration token from backend
#define REGISTRATION_TOKEN_CUUID "cb520851-1fd3-446e-a590-e777ddd0232c"

// Api url
#define API_URL_CUUID "20418184-e336-4409-a04d-61d7cf31f56b"

// Station name
#define STATION_NAME_CUUID "23d334ae-161c-4024-a634-57b781fde853"

// Station address
#define STATION_COUNTRY_CUUID "cb5f7871-ad3c-4d60-85c6-ca48b5d70550"
#define STATION_CITY_CUUID "fe070113-c0c5-4237-b907-fdc7eb6b4cd9"
#define STATION_STREET_CUUID "171a79f2-3f0b-4a21-9d6c-12dd318d1582"
#define STATION_NUMBER_CUUID "1ed02261-8571-45d9-9cdf-7092b2a315e8"

// GPS Location
#define LOCATION_LATITUDE_CUUID "394bf3e9-df5d-4765-9a47-e1fd722ae0cb"
#define LOCATION_LONGITUDE_CUUID "cca719aa-7cf0-45f2-b2b6-dba82e0d62ab"
#define LOCATION_MANUALLY_CUUID "54ef86d9-e6b5-42ba-a4a2-518f93350eb2"

// Device state - determines if all sensors are working
#define DEVICE_STATE_CUUID "f363fc8f-92dd-4e0b-ae26-90e3e17e6560"

// Connection state - determines if connection to internet services is ok
#define CONNECTION_STATE_CUUID "ba71dcda-609f-4b3e-8095-11662afa4c5f"

// Registration state - determines if registration process has finished/failed
#define REGISTRATION_STATE_CUUID "176959dd-c1f5-4dac-88b8-1f28977fa7ef"

// Refreshs device when any message is received
#define REFRESH_DEVICE_CUUID "2ca7df70-42df-482e-81cf-468e0fcec5dc"

// Clears device when any message is received
#define CLEAR_DATA_CUUID "9023e6f3-223d-4c6c-bd39-ebca35d7e8d0"

// BT chunker test
#define TEST_CHUNK_UPLOAD_CUUID "9023e6f3-223d-4c6c-bd39-ebca35d7e8d1"
#define TEST_CHUNK_DOWNLOAD_CUUID "9023e6f3-223d-4c6c-bd39-ebca35d7e8d2"

#define DEFAULT_BT_PERMISSION ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED

class Bluetooth {
 public:
  static void start(BluetoothHandler *bluetoothHandler);
  static void reloadValues();
  static BluetoothHandler *bluetoothHandler;
  static String getLastOperationStatus();
  static void setLastOperationStatus(String operationStatus);

  static const uint32_t W_PROPERTY;
  static const uint32_t R_PROPERTY;
  static const uint32_t RW_PROPERTY;

 private:
  static BLECharacteristic *stationNameCharacteristic;
  static BLECharacteristic *stationCountryCharacteristic;
  static BLECharacteristic *stationCityCharacteristic;
  static BLECharacteristic *stationStreetCharacteristic;
  static BLECharacteristic *stationNumberCharacteristic;
  static BLECharacteristic *latitudeCharacteristic;
  static BLECharacteristic *longitudeCharacteristic;
  static BLECharacteristic *locationManualCharacteristic;

  static BLECharacteristic *devPasswordCharacteristic;
  static BLECharacteristic *inetConnTypeCharacteristic;
  static BLECharacteristic *registrationStateCharacteristic;
  static BLECharacteristic *ssidCharacteristic;
  static BLECharacteristic *wifiPassCharacteristic;
  static BLECharacteristic *registerTokenCharacteristic;
  static BLECharacteristic *apiUrlCharacteristic;
  static BLECharacteristic *devStateCharacteristic;
  static BLECharacteristic *connStateCharacteristic;
  static BLECharacteristic *refreshDeviceCharacteristic;
  static BLECharacteristic *clearDataCharacteristic;

  static BluetoothChunkReceiver* chunkerTestUploadCharacteristic;

  static String lastOperatioinState;
};