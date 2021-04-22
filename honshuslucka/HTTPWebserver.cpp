#include <Arduino.h>
#include "HatchRequestAction.h"
#include "HTTPWebServer.h"
#include "TextMessageGenerator.h"

#include "webpages/axios_js.h"
#include "webpages/controls_html.h"
#include "webpages/info_html.h"
//#include "webpages/javascriptParameters_js.h"
#include "webpages/knockout_js.h"
#include "webpages/paho_js.h"
#include "webpages/simple_css.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

HTTPWebServer::HTTPWebServer(HatchRequest& hatchRequest, ESP8266WebServer& server, TextMessageGenerator& tMG, DistanceMeterData& currentMeterData, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword, String mqttPublishTopicActuatorAction, String mqttPublishTopicHatchLidarDistanceToObjectCm, String mqttPublishTopicHatchLidarStrengthOrQualityOfReturnSignal, String mqttPublishTopicHatchLidarTemperatureInternalOfLidarSensorChipCelsius)
    :   _hatchRequest(hatchRequest),
        _server(server),
        _tMG(tMG),
        _currentMeterData(currentMeterData)
{    
    _mqttBrokerURL = mqttBrokerURL;
    _mqttPort = mqttPort;
    _mqttUsername = mqttUsername;
    _mqttPassword = mqttPassword;
    
    _mqttPublishTopicActuatorAction = mqttPublishTopicActuatorAction;
    _mqttPublishTopicHatchLidarDistanceToObjectCm = mqttPublishTopicHatchLidarDistanceToObjectCm;
    _mqttPublishTopicHatchLidarStrengthOrQualityOfReturnSignal = mqttPublishTopicHatchLidarStrengthOrQualityOfReturnSignal;
    _mqttPublishTopicHatchLidarTemperatureInternalOfLidarSensorChipCelsius = mqttPublishTopicHatchLidarTemperatureInternalOfLidarSensorChipCelsius;

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
    _server.on("/javascriptKnockout_js", [this]() { routeGetJavascriptKnockoutJs(); });
    _server.on("/javascriptPaho_js", [this]() { routeGetJavascriptPahoJs(); });
    _server.on("/constJavascriptParameters_js", [this]() { routeGetConstJavascriptParameters(); });

    _server.on("/styleSimple_css", [this]() { routeGetStyleSimpleCss(); });


    _server.on("/openHatch", [this]() { routeGetOpenHatch(); });
    _server.on("/closeHatch", [this]() { routeGetCloseHatch(); });

    _server.on("/pushActuator", [this]() { routeGetPushActuator(); });
    _server.on("/stopActuator", [this]() { routeGetStopActuator(); });
    _server.on("/pullActuator", [this]() { routeGetPullActuator(); });

    _server.on("/lidarSensorData", [this]() { routeGetLidarSensorData(); });

    _server.onNotFound([this]() { routeGetNotFound(); });
}

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

void HTTPWebServer::routeGetJavascriptKnockoutJs() {
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(KNOCKOUT_JS) - 1;
    _server.send_P(200, "text/javascript", KNOCKOUT_JS, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetJavascriptPahoJs() {
    const unsigned long SIZE_WITHOUT_TERMINATING_NULL_CHARACTER = sizeof(PAHO_JS) - 1;
    _server.send_P(200, "text/javascript", PAHO_JS, SIZE_WITHOUT_TERMINATING_NULL_CHARACTER);
}

void HTTPWebServer::routeGetConstJavascriptParameters() {
    
    char wifiChannel[6];
    itoa(WiFi.channel(), wifiChannel, 10);

    const String constJavascriptParameters = 
        String("const constJavascriptParameters = {") +
            String("'device' : {") +
                    String("'firmwareVersion': '") + _tMG.firmwareVersion() + ("',") +
                    String("'macAddress': '") + WiFi.macAddress() + ("',") +
                    String("'serialMonitorBaud': '") + _tMG.serialMonitorBaud() + ("',") +
            String("},") +

            String("'mqtt' : {") +
                String("'publishTopics': {") +
                    String("'actuatorAction': '") + _mqttPublishTopicActuatorAction + ("',") +
                    String("'lidarDistanceToObjectCm': '") + _mqttPublishTopicHatchLidarDistanceToObjectCm + ("',") +
                    String("'lidarStrengthOrQualityOfReturnSignal': '") + _mqttPublishTopicHatchLidarStrengthOrQualityOfReturnSignal + ("',") +
                    String("'lidarTemperatureInternalOfLidarSensorChipCelsius': '") + _mqttPublishTopicHatchLidarTemperatureInternalOfLidarSensorChipCelsius + ("',") +
                String("},") +
                String("'hostname': '") + _mqttBrokerURL + ("',") +
                String("'clientId': 'clie4ntId',") +
                String("'connectionOptions': {") +
                    String("'userName': '") + _mqttUsername + ("',") +
                    String("'password': '") + _mqttPassword + ("',") +
                    String("'keepAliveInterval': 60,") +
                String("},") +
                String("'port':") + String(_mqttPort) +
            String("},") +
            
            String("'wifi' : {") +
                    String("'channel': '") + wifiChannel + ("',") +
                    String("'localIP': '") + WiFi.localIP().toString() + ("',") +
                    String("'SSID': '") + WiFi.SSID() + ("',") +
            String("}") +
        String("};");
    _server.send_P(200, "text/javascript", constJavascriptParameters.c_str());
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