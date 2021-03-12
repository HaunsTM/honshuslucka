#include <string>

#include "TextMessageGenerator.h"

#ifndef HTTP_WEBSERVER_H
#define HTTP_WEBSERVER_H

class HTTPWebserver {

private:
    int _webserverportPort;

    bool _initialized;

    TextMessageGenerator& _tMG;
public:
    HTTPWebserver(TextMessageGenerator& tMG);
    
    ~HTTPWebserver();

    void initialize();

    bool isConnectedToWifi();

};

#endif