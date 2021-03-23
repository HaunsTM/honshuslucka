#include <Arduino.h>
#include <string>
#include <ESP8266WebServer.h>
#include "ActionRequest.h"

#include "TextMessageGenerator.h"

#ifndef HTTP_WEB_SERVER_H
#define HTTP_WEB_SERVER_H

class HTTPWebServer {

private:
    int _webserverPort;

    bool _initialized;
    ActionRequest& _actionRequest;
    ESP8266WebServer& _server;
    TextMessageGenerator& _tMG;


    String htmlEnveloper(String title, String bodyContent);

    String javascriptAddActuatorControlButtonEventListeners();
    String javascriptCallAPIFromButton();

    void setUpRouteHandlers();

    void routeGetControl();
    void routeGetInfo();
    void routeGetNotFound();

    void routeActuatorOpenHatch();
    void routeGetActuatorCloseHatch();

    void routeActuatorPush();
    void routeActuatorTurnOff();
    void routeActuatorPull();
public:
    HTTPWebServer(ActionRequest& actionRequest, ESP8266WebServer& server, TextMessageGenerator& tMG);
    
    ~HTTPWebServer();

    void initialize();

    bool isConnectedToWifi();
    void handleClient();

};

#endif