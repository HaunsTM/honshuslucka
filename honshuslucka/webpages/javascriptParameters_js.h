
#ifndef JAVASCRIPT_PARAMETERS_H
#define JAVASCRIPT_PARAMETERS_H


const char JAVASCRIPT_PARAMETERS[] PROGMEM = R"=====(
    const mqttParameters = {
        'subscriptionTopics': {
            'lidarDistanceToObjectCm': 'iot/hen_house/hatch/lidar/distanceToObjectCm',
            'actuatorAction': 'iot/hen_house/hatch/actuator/action'
        },
        'hostname': '10.0.0.6',
        'clientId': 'clie4ntId',
        'connectionOptions': {
            'userName': 'mqtt1',
            'password': 'mqtt1',
            'keepAliveInterval': 30,
        },
        'port':1884
    };)=====";

#endif