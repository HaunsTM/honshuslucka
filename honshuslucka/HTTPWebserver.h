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

    String _mqttBrokerURL;
    int _mqttPort;
    String _mqttUsername;
    String _mqttPassword;
    
    String baseMQTTHen_HouseHatchTopic();
    String htmlEnveloper(String title, String bodyContent);

    void setUpRouteHandlers();

    void routeGetJavascriptAxiosJs();
    void routeGetJavascriptParameters();
    void routeGetJavascriptPahoJs();
    
    void routeGetStyleSimpleCss();

    void routeGetControls();
    void routeGetInfo();
    void routeGetNotFound();

    void routeGetOpenHatch();
    void routeGetCloseHatch();

    void routeGetPushActuator();
    void routeGetStopActuator();
    void routeGetPullActuator();
    
    void routeGetLidarSensorData();
public:
    HTTPWebServer(HatchRequest& hatchRequest, ESP8266WebServer& server, TextMessageGenerator& tMG, DistanceMeterData& currentMeterData, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword);
    
    ~HTTPWebServer();

    void initialize();

    bool isConnectedToWifi();
    void handleClient();

};

#endif