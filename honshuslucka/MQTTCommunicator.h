

#include "ActuatorAction.h"
#include "DistanceMeterData.h"
#include <PubSubClient.h>
#include "TextMessageGenerator.h"

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

    ActuatorAction& _m;
    PubSubClient& _pubSubClient;
    TextMessageGenerator& _tMG;
    String baseReportHen_HouseHatchTopic();
public:
    MQTTCommunicator(PubSubClient& pubSubClient, ActuatorAction& m, TextMessageGenerator& tMG, String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword, String mqttTopicSubscribe);
    
    ~MQTTCommunicator();

    void initialize();

    bool isConnectedToWifi();

    bool isConnectedToMQTTBroker();

    void mqttReceived(char* topic, byte* payload, unsigned int length);

    void connectToMQTTBroker();

    void reportHen_HouseHatchLidar(DistanceMeterData& currentMeterData);

};

#endif