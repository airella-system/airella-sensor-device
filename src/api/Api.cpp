#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include <ArduinoJson.h>

bool ApiClass::registerSensor(const char *type) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/sensors";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["type"] = type;
  doc["id"] = type;
  String body = "";
  serializeJson(doc, body);

  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("Sensor add response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 201) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::updateAccessToken() {
  if (this->accessToken.equals("") ||
      (millis() - this->accessTokenMillis) > ACCESS_TOKEN_EXPIRATION_TIME) {
    String registrationToken = Config::instance().getRegistratonToken();
    String apiUrlBase = Config::instance().getApiUrl();
    String url = apiUrlBase + "/auth/refresh-token";
    String refreshToken = Config::instance().getRefreshToken();

    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
    doc["refreshToken"] = refreshToken;
    String body = "";
    serializeJson(doc, body);

    Http::Response response = Internet::httpPost(url, body);

    String debugText = String("Get token response code: ") + response.code +
                       " payload: " + response.payload;
    Logger::debug(debugText.c_str());
    if (response.code == 200) {
      DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) +
                              280);
      deserializeJson(doc, response.payload);
      const char *token = doc["data"]["accessToken"]["token"];
      this->accessToken = String(token);
      this->accessTokenMillis = millis();
      return true;
    }
    return false;
  } else {
    return true;
  }
}

bool ApiClass::publishName(const char *name) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/name";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
  doc["name"] = name;
  String body = "";
  serializeJson(doc, body);

  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set name response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::publishLocation(double latitude, double longitude) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/location";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["latitude"] = String(latitude, 6);
  doc["longitude"] = String(longitude, 6);
  String body = "";
  serializeJson(doc, body);

  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::publishAddress(const char *country, const char *city,
                              const char *street, const char *number) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/address";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["country"] = country;
  doc["city"] = city;
  doc["street"] = street;
  doc["number"] = number;
  String body = "";
  serializeJson(doc, body);

  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::registerStation() {
  Config::instance().setRegistrationState(Config::RegistrationState::REGISTERING);

  String registrationToken = Config::instance().getRegistratonToken();
  String apiUrlBase = Config::instance().getApiUrl();
  String url = apiUrlBase + "/auth/register-station";

  const size_t capacity = JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);
  doc["stationRegistrationToken"] = registrationToken;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("Registraton response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code != 201) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }

  DynamicJsonDocument doc(2*JSON_OBJECT_SIZE(2) + 120);
  deserializeJson(doc, response.payload);
  const char *id = doc["data"]["id"];
  const char *refreshToken = doc["data"]["refreshToken"];
  Config::instance().setApiStationId(String(id));
  Config::instance().setRefreshToken(String(refreshToken));

  if(!updateAccessToken()) {
      Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
      return false;
  }

  String name = Config::instance().getStationName();
  if (!name.equals("")) {
    if(!publishName(name.c_str())) {
      Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
      return false;
    }
  }
  publishAddress(Config::instance().getAddressCountry().c_str(),
      Config::instance().getAddressCity().c_str(),
      Config::instance().getAddressStreet().c_str(),
      Config::instance().getAddressNumber().c_str());

  if (!registerSensor("temperature")) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("humidity")) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm1")) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm2_5")) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm10")) {
    Config::instance().setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }

  Config::instance().setRegistrationState(Config::RegistrationState::REGISTERED);
  return true;
}

bool ApiClass::isRegistered() {
  return Config::instance().getRegistrationState() == Config::RegistrationState::REGISTERED;
}

bool ApiClass::publishMeasurement(String sensor, double value) {
  if (this->isRegistered() && this->updateAccessToken()) {
    String apiUrlBase = Config::instance().getApiUrl();
    String url = apiUrlBase + "/stations/" +
                 Config::instance().getApiStationId() + "/sensors/" + sensor +
                 "/measurements";

    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);
    doc["value"] = String(value);
    String body = "";
    serializeJson(doc, body);
    
    Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
    Http::Response response = Internet::httpPost(url, body);

    String debugText = String("Add measurement response code: ") +
                       response.code + " payload: " + response.payload;
    Logger::debug(debugText.c_str());

    if (response.code == 200) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void ApiClass::configUpdated() {
  Internet::stop();
  Internet::start();

  if (!this->isRegistered() &&
      !Config::instance().getRegistratonToken().equals("")) {
    this->registerStation();
    Config::instance().save();
  }
}

ApiClass Api;