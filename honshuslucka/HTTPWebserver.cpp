#include <Arduino.h>
#include "HTTPWebserver.h"
#include "TextMessageGenerator.h"

#include <ESP8266WiFi.h>

HTTPWebserver::HTTPWebserver(TextMessageGenerator& tMG)
    :
        _tMG(tMG)
{

    _initialized = false;
}

HTTPWebserver::~HTTPWebserver() {
}


bool HTTPWebserver::isConnectedToWifi() {
    bool isConnected = WiFi.status() == WL_CONNECTED;
    return isConnected;
}

void HTTPWebserver::initialize() {
    if (!_initialized) {
        // https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/


        _initialized = true;
    }
}
