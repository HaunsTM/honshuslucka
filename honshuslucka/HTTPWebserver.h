#include <Arduino.h>
#include "DistanceMeter.h"
#include <ESP8266WebServer.h>
#include "HatchRequest.h"

#include "TextMessageGenerator.h"

#ifndef HTTP_WEB_SERVER_H
#define HTTP_WEB_SERVER_H

class HTTPWebServer {

private:
    int _webserverPort;

    bool _initialized;
    HatchRequest& _hatchRequest;
    ESP8266WebServer& _server;
    TextMessageGenerator& _tMG;
    DistanceMeterData& _currentMeterData;

    String htmlEnveloper(String title, String bodyContent);

    String javascriptAddActuatorControlButtonEventListeners();
    String javascriptCallAPIFromButton();

    void setUpRouteHandlers();

    void routeGetControl();
    void routeGetInfo();
    void routeGetNotFound();

    void routeGetOpenHatch();
    void routeGetCloseHatch();

    void routeGetPushActuator();
    void routeGetStopActuator();
    void routeGetPullActuator();
    
    void routeGetLidarSensorData();
public:
    HTTPWebServer(HatchRequest& hatchRequest, ESP8266WebServer& server, TextMessageGenerator& tMG, DistanceMeterData& currentMeterData);
    
    ~HTTPWebServer();

    void initialize();

    bool isConnectedToWifi();
    void handleClient();

};

#endif