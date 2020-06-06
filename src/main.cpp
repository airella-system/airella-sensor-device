#include <Arduino.h>
#include <ArduinoJson.h>
#include "config/Config.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "maintenance/Logger.h"
#include "api/Api.h"

bool refreshRequested = false;

class MyBluetoothHandler : public BluetoothHandler
{
  void deviceRefreshRequest()
  {
    Serial.println("Refresh request!");
    refreshRequested = true;
  }
};

void setup()
{
  Serial.begin(115200);
  Config::instance().load();
  Bluetooth::start(new MyBluetoothHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();
}

void loop()
{
  if (refreshRequested)
  {
    Config::instance().save();
    Api.configUpdated();
    refreshRequested = false;
  }

  delay(1000);
}
