#pragma once

#include <Arduino.h>

class Http
{
public:
    struct Response
    {
        int code;
        String payload;
    };
};