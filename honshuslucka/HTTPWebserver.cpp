#include <Arduino.h>
#include "HatchRequestAction.h"
#include "HTTPWebServer.h"
#include "TextMessageGenerator.h"

#include "webpages/axios_js.h"
#include "webpages/controls_html.h"
#include "webpages/info_html.h"
#include "webpages/javascriptParameters_js.h"
#include "webpages/paho_js.h"
#include "webpages/simple_css.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

HTTPWebServer::HTTPWebServer(HatchRequest& hatchRequest, ESP8266WebServer& server, TextMessageGenerator& tMG, DistanceMeterData& currentMeterData, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword)
    :   _hatchRequest(hatchRequest),
        _server(server),
        _tMG(tMG),
        _currentMeterData(currentMeterData)
{    
    _mqttBrokerURL = mqttBrokerURL;
    _mqttPort = mqttPort;
    _mqttUsername = mqttUsername;
    _mqttPassword = mqttPassword;

    _initialized = false;
}

HTTPWebServer::~HTTPWebServer() {
}


bool HTTPWebServer::isConnectedToWifi() {
    bool isConnected = WiFi.status() == WL_CONNECTED;
    return isConnected;
}

void HTTPWebServer::initialize() {
    if (!_initialized) {
        // https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/


        _hatchRequest.setAction(HatchRequestAction::STOP);
        _hatchRequest.setAcknowledged(true);

        setUpRouteHandlers();
        _server.begin();

        _initialized = true;
    }
}

void HTTPWebServer::setUpRouteHandlers() {

    _server.on("/", [this]() { routeGetControls(); });

    _server.on("/info", [this]() { routeGetInfo(); });
    
    _server.on("/javascriptAxios_js", [this]() { routeGetJavascriptAxiosJs(); });
    _server.on("/javascriptParameters_js", [this]() { routeGetJavascriptParameters(); });
    _server.on("/javascriptPaho_js", [this]() { routeGetJavascriptPahoJs(); });

    _server.on("/styleSimple_css", [this]() { routeGetStyleSimpleCss(); });


    _server.on("/openHatch", [this]() { routeGetOpenHatch(); });
    _server.on("/closeHatch", [this]() { routeGetCloseHatch(); });

    _server.on("/pushActuator", [this]() { routeGetPushActuator(); });
    _server.on("/stopActuator", [this]() { routeGetStopActuator(); });
    _server.on("/pullActuator", [this]() { routeGetPullActuator(); });

    _server.on("/lidarSensorData", [this]() { routeGetLidarSensorData(); });

    _server.onNotFound([this]() { routeGetNotFound(); });
}

String HTTPWebServer::htmlEnveloper(String title, String bodyContent) {
    const String localIP = WiFi.localIP().toString();
    const String html =
        String("<!DOCTYPE html>") +
        String("<html>") +
            String("<head>") +
                String("<title>") + title + String("</title>") +
                String("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">") +
                String("<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">") +
                String("<style>") +
                    String("body {font-family: Arial, sans-serif; color: Black; padding: 1rem;}") +
                    String("table {width: 70vw; font-family: 'Courier New', monospace; padding: 1rem 0;}") +
                    String("table thead tr:first-child th {font-weight: bold; background-color: LightGrey;}") +
                    String("table thead tr:last-child th {border-bottom: 1px solid LightGrey;}") +
                    String(".table-sub-head { text-align:center; background-color: LightGrey;}") +
                    String("td a { font-size: 0.8rem;}") +
                    String(".button-symbol {width: 3rem; height: 3rem; font-size: 2rem; font-weight: bold;}") +
                    String(".button-success {background-color: LightGreen;}") +
                    String(".button-error {background-color: Tomato;}") +
                    String(".info { text-align: right; border-top: 1px solid DarkGray; color: Gray; font-style: italic; font-size: small;}") +
                    String(".title { border-top: 1px solid Black;}") +
                    String(".response-result { font-size: 0.8rem; font-family: 'Courier New', monospace; padding: 0 1rem;}") +
                    String(".navigation-links { display: flex; justify-content: flex-end;}") +
                String("</style>") +
            String("</head>") +
            String("<body>") +
                String("<h1 class=\"title\">") + title + String("</h1>") +
                bodyContent +
                String("<nav class=\"navigation-links\">") +
                    String("<a href=\"//") + localIP + String("/\">Control</a>   |   ") +
                    String("<a href=\"//") + localIP + String("/info\">Device info</a>") +
                String("</nav>") +
                String("<div class=\"info\">") +
                    String("<p>This software is distributed under <a href = \"https://en.wikipedia.org/wiki/MIT_License\">MIT License</a>. Source code on <a href=\"https://github.com/HaunsTM\">Github - HaunsTM</a></p>") +
                String("</div>") +
            String("</body>") +
        String("</html>");
    return html;
};

void HTTPWebServer::routeGetControls() {

    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(CONTROLS_HTML) - 1;
    _server.send_P(200, "text/html", CONTROLS_HTML, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
    
}

String HTTPWebServer::baseMQTTHen_HouseHatchTopic() {
    return "<code>iot/hen_house/hatch/</code>";
};

void HTTPWebServer::routeGetInfo() {    
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(INFO_HTML) - 1;
    _server.send_P(200, "text/html", INFO_HTML, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetJavascriptAxiosJs() {
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(AXIOS_JS) - 1;
    _server.send_P(200, "text/javascript", AXIOS_JS, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetJavascriptParameters() {
    const String javascriptParameters = 
        String("const parameters = {") +
            String("'mqtt' : {") +
                String("'subscriptionTopics': {") +
                    String("'lidarDistanceToObjectCm': 'iot/hen_house/hatch/lidar/distanceToObjectCm',") +
                    String("'actuatorAction': 'iot/hen_house/hatch/actuator/action'") +
                String("},") +
                String("'hostname': '") + _mqttBrokerURL + ("',") +
                String("'clientId': 'clie4ntId',") +
                String("'connectionOptions': {") +
                    String("'userName': '") + _mqttUsername + ("',") +
                    String("'password': '") + _mqttPassword + ("',") +
                    String("'keepAliveInterval': 30,") +
                String("},") +
                String("'port':") + String(_mqttPort) +
            String("},") +
        String("};");

    _server.send_P(200, "text/javascript", javascriptParameters.c_str());
}

void HTTPWebServer::routeGetJavascriptPahoJs() {
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(PAHO_JS) - 1;
    _server.send_P(200, "text/javascript", PAHO_JS, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetStyleSimpleCss() {
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(SIMPLE_CSS) - 1;
    _server.send_P(200, "text/css", SIMPLE_CSS, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetOpenHatch() {
    _hatchRequest.setAction(HatchRequestAction::OPEN_HATCH);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Opening hatch");
}

void HTTPWebServer::routeGetCloseHatch() {
    _hatchRequest.setAction(HatchRequestAction::CLOSE_HATCH);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Closing hatch");
}

void HTTPWebServer::routeGetPushActuator() {
   _hatchRequest.setAction(HatchRequestAction::DOWN);
   _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Extends actuator...");
}

void HTTPWebServer::routeGetStopActuator() {
    _hatchRequest.setAction(HatchRequestAction::STOP);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Turned off actuator");
}

void HTTPWebServer::routeGetPullActuator() {
    _hatchRequest.setAction(HatchRequestAction::UP);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Contracting actuator...");
}

void HTTPWebServer::routeGetLidarSensorData() {

    String lidarSensorData = 
        String("{") +
            String("\"distanceToObjectCm\":") + String(_currentMeterData.distanceToObjectCm)  + String(",") +
            String("\"strengthOrQualityOfReturnSignal\":") + String(_currentMeterData.strengthOrQualityOfReturnSignal)  + String(",") +
            String("\"temperatureInternalOfLidarSensorChipCelsius\":") + String(_currentMeterData.temperatureInternalOfLidarSensorChipCelsius)   +
        String("}");
    _server.send(200, "application/json", lidarSensorData.c_str());
}

void HTTPWebServer::routeGetNotFound() {
    _server.send(404, "text/plain", "Not found");
}

void HTTPWebServer::handleClient() {
    _server.handleClient();
}