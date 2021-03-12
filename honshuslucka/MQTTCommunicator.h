#include <string>

#include "MqttAction.h"
#include "TextMessageGenerator.h"
#include <PubSubClient.h>

#ifndef MQTT_COMMUNICATOR_H
#define MQTT_COMMUNICATOR_H

class MQTTCommunicator {

private:

    String _mqttBrokerURL;
    int _mqttPort;
    String _mqttUsername;
    String _mqttPassword;
    String _mqttTopicSubscribe;

    bool _initialized;

    MqttAction& _m;
    PubSubClient& _pubSubClient;
    TextMessageGenerator& _tMG;
public:
    MQTTCommunicator(PubSubClient& pubSubClient, MqttAction& m, TextMessageGenerator& tMG, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword, String mqttTopicSubscribe);
    
    ~MQTTCommunicator();

    void initialize();

    bool isConnectedToWifi();

    bool isConnectedToMQTTBroker();

    void mqttReceived(char* topic, byte* payload, unsigned int length);

    void connectToMQTTBroker();

};

#endif