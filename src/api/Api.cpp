#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include <ArduinoJson.h>

bool ApiClass::registerSensor(const char *type) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/sensors";
  String body =
      String("{ \"type\": \"") + type + "\", \"id\": \"" + type + "\" }";
  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("Sensor add response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());
}

bool ApiClass::updateAccessToken() {
  if (this->accessToken.equals("") ||
      (millis() - this->accessTokenMillis) > ACCESS_TOKEN_EXPIRATION_TIME) {
    String registrationToken = Config::instance().getRegistratonToken();
    String apiUrlBase = Config::instance().getApiUrl();
    String url = apiUrlBase + "/auth/refresh-token";
    String refreshToken = Config::instance().getRefreshToken();
    String body = "{ \"refreshToken\": \"" + refreshToken + "\" }";
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
  String body = String("{ \"name\": \"") + name + "\" }";
  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set name response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());
}

bool ApiClass::publishLocation(double latitude, double longitude) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/location";
  String body = String("{ \"latitude\": ") + String(latitude, 6) +
                ", \"longitude\": " + String(longitude, 6) + " }";
  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());
}

bool ApiClass::publishAddress(const char *country, const char *city,
                              const char *street, const char *number) {
  String apiUrlBase = Config::instance().getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::instance().getApiStationId() + "/address";
  String body = String("{ \"country\": \"") + country + "\", \"city\": \"" +
                city + "\", \"street\": \"" + street + "\", \"number\": \"" +
                number + "\" }";
  Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
  Http::Response response = Internet::httpPut(url, body);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());
}

bool ApiClass::registerStation() {
  String registrationToken = Config::instance().getRegistratonToken();
  String apiUrlBase = Config::instance().getApiUrl();
  String url = apiUrlBase + "/auth/register-station";
  String body =
      "{ \"stationRegistrationToken\": \"" + registrationToken + "\" }";
  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("Registraton response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 201) {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, response.payload);
    const char *id = doc["data"]["id"];
    const char *refreshToken = doc["data"]["refreshToken"];
    Config::instance().setApiStationId(String(id));
    Config::instance().setRefreshToken(String(refreshToken));

    updateAccessToken();

    int randomNumber = random(1, 9999999);
    publishName((String("Mock sensor ") + String(randomNumber)).c_str());
    publishAddress("Poland", "Kraków", "Mockowa", String(randomNumber).c_str());

    double latitudeMin = 49.972368;
    double latitudeMax = 50.137422;
    double longitudeMin = 20.435403;
    double longitudeMax = 20.735403;

    double latitude =
        latitudeMin + random(1, (latitudeMax - latitudeMin) * 100000) / 100000;
    double longitude =
        longitudeMin +
        random(1, (longitudeMax - longitudeMin) * 100000) / 100000;

    publishLocation(latitude, longitude);

    registerSensor("temperature");
    registerSensor("humidity");
    registerSensor("pm1");
    registerSensor("pm2_5");
    registerSensor("pm10");
    return true;
  } else {
    return false;
  }
}

bool ApiClass::isRegistered() {
  return !Config::instance().getRefreshToken().equals("") &&
         !Config::instance().getApiStationId().equals("");
}

bool ApiClass::publishMeasurement(String sensor, double value) {
  if (this->isRegistered() && this->updateAccessToken()) {
    String apiUrlBase = Config::instance().getApiUrl();
    String url = apiUrlBase + "/stations/" +
                 Config::instance().getApiStationId() + "/sensors/" + sensor +
                 "/measurements";
    String body = String("{ \"value\": " + String(value) + " }");
    Internet::setAuthorizationHeader(String("Bearer ") + accessToken);
    Http::Response response = Internet::httpPost(url, body);

    String debugText = String("Add measurement response code: ") +
                       response.code + " payload: " + response.payload;
    Logger::debug(debugText.c_str());
  }
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