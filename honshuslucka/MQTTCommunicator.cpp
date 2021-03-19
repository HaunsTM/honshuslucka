#include <Arduino.h>
#include "MQTTCommunicator.h"
#include "TextMessageGenerator.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

MQTTCommunicator::MQTTCommunicator(PubSubClient& pubSubClient, ActuatorAction& m, TextMessageGenerator& tMG, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword, String mqttTopicSubscribe)
    :   
        _pubSubClient(pubSubClient),
        _m(m),
        _tMG(tMG)
{
    _mqttBrokerURL = mqttBrokerURL;
    _mqttPort = mqttPort;
    _mqttUsername = mqttUsername;
    _mqttPassword = mqttPassword;
    _mqttTopicSubscribe = mqttTopicSubscribe;

    _initialized = false;
}

MQTTCommunicator::~MQTTCommunicator() {
}


bool MQTTCommunicator::isConnectedToWifi() {
    bool isConnected = WiFi.status() == WL_CONNECTED;
    return isConnected;
}

bool MQTTCommunicator::isConnectedToMQTTBroker() {
    bool isConnected = _pubSubClient.connected();
    return isConnected;
}

void MQTTCommunicator::initialize() {
    if (!_initialized) {
        // https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/

        _pubSubClient.setServer(_mqttBrokerURL.c_str(), _mqttPort);
        String s;
        char buf[12];
        s += String("_mqttBrokerURL:  ") + _mqttBrokerURL.c_str() + String("_mqttPort:  ") + itoa(_mqttPort, buf, 10) + String("\n");
        //Serial.println(s);

        // 
        auto mqttReceived = [&](char* topic, byte* payload, unsigned int length) { 
            Serial.println("mqtt received");
            switch ((char)payload[0]) {
                case '0':
                    _m = ActuatorAction::pull;
                    break;
                case '1':
                    _m = ActuatorAction::turnOff;
                    break;
                case '2':
                    _m = ActuatorAction::push;
                    break;
                default:
                    _m = ActuatorAction::turnOff;
                    break;
            } 
        };

        _pubSubClient.setCallback(mqttReceived);

        _initialized = true;
    }
}

void MQTTCommunicator::mqttReceived(char* topic, byte* payload, unsigned int length) {
    //Serial.print(tMG.messageArrived(topic, payload, length).c_str());

    switch ((char)payload[0]) {
    case '0':
        _m = ActuatorAction::pull;
        break;
    case '1':
        _m = ActuatorAction::turnOff;
        break;
    case '2':
        _m = ActuatorAction::push;
        break;
    default:
        _m = ActuatorAction::turnOff;
        break;
    }
}

void MQTTCommunicator::connectToMQTTBroker() {
    // make sure we are notr connected already
    if (!_pubSubClient.connected()) {
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        Serial.print(_tMG.connectingToMQTTServer(clientId.c_str()).c_str());

        // Attempt to connect
        if (_pubSubClient.connect(clientId.c_str(), _mqttUsername.c_str(), _mqttPassword.c_str())) {

            Serial.print(_tMG.mQTTServerConnectionEstablished().c_str());
            // Once connected, publish an announcement...
            //_pubSubClient.publish(SETTINGS_DATA_MQTT_TOPIC_PUBLISH.c_str(), "hello world");

            // ... and resubscribe
            _pubSubClient.subscribe(_mqttTopicSubscribe.c_str());
        }
        else {
            Serial.print(_tMG.mQTTServerConnectionFailed(_pubSubClient.state()).c_str());
        }
    }
}
