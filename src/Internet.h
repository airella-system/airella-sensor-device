#pragma once

#include <Arduino.h>
#include "WiFiConn.h"
#include "GsmConn.h"
#include "Http.h"

class Internet
{

public:
    enum Type
    {
        WIFI = 0,
        GSM = 1
    };

    static void setType(Type type);
    static int start();
    static void stop();
    static Http::Response httpGet(String url);
    static Http::Response httpPost(String url, String json);
    static Http::Response httpPut(String url, String json);

private:
    static Type type;
};