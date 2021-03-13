#include <Arduino.h>
#include <string>
#include <ESP8266WebServer.h>

#include "TextMessageGenerator.h"

#ifndef HTTP_WEB_SERVER_H
#define HTTP_WEB_SERVER_H

class HTTPWebServer {

private:
    int _webserverPort;

    bool _initialized;
    ESP8266WebServer& _server;
    TextMessageGenerator& _tMG;


    String htmlEnveloper(String title, String bodyContent);

    String javascriptCallAPIFromButton();

    void setUpRouteHandlers();

    void routeGetInfo();
    void routeGetNotFound();
public:
    HTTPWebServer(ESP8266WebServer& server, TextMessageGenerator& tMG);
    
    ~HTTPWebServer();

    void initialize();

    bool isConnectedToWifi();
    void handleClient();

};

#endif