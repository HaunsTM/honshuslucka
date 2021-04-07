#include "Actuator.h"
#include "ActuatorAction.h"
#include "Blinker.h"
#include "DistanceMeter.h"
#include "HTTPWebServer.h"
#include "HatchRequest.h"
#include "MQTTCommunicator.h"
#include "OnboardLED.h"
#include "Pins.h"
#include "SettingsData.h"
#include "Switch.h"
#include "SwitchesManager.h"
#include "TextMessageGenerator.h"
#include "WifiManager.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.1

TFMPlus tfmP;         // Create a TFMini Plus object  
SoftwareSerial mySerial(PIN_D2_GPIO4_SDA,PIN_D1_GPIO5_SCL); //define software serial port name as mySerial and define D5 as RX and D0 as TX
DistanceMeterData currentMeterData;
DistanceMeterData previousMeterData;
DistanceMeter distanceMeter(mySerial, currentMeterData);

enum class HatchLocomotion {
    UP,
    NO_CHANGE,
    DOWN
};

HatchLocomotion actualHatchLocomotion;
HatchLocomotion previousHatchLocomotion;

OnboardLED onboardLED;
Blinker blinker(onboardLED);

Actuator actuator(PIN_D6_GPIO12_MISO, PIN_D7_GPIO13_MOSI);

Switch actuatorPullButton(PIN_D3_GPIO0_FLASH, true);
Switch actuatorPushButton(PIN_D4_GPIO2, true);

TextMessageGenerator tMG(
    SETTINGS_DATA_FIRMWARE_VERSION, SETTINGS_DATA_SERIAL_MONITOR_BAUD,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD);

HatchRequest hatchRequests[2] = { HatchRequest("switch button requested"), HatchRequest("http requested") };

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);
ESP8266WebServer eSP8266WebServer(SETTINGS_DATA_WEB_SERVER_PORT);

void initiateRequestArray() {
    hatchRequests[0].initialize();
    hatchRequests[1].initialize();
}

SwitchesManager switchesManager(hatchRequests[0], actuatorPullButton, actuatorPushButton);

WifiManager wifiManager( tMG, CREDENTIALS_JSON_STRING);

HTTPWebServer webserver(hatchRequests[1], eSP8266WebServer, tMG, currentMeterData);

ActuatorAction m;
MQTTCommunicator mQTTC(
    pubSubClient, m, tMG,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD);

enum class ChickenHatchStates {
    STAND_BY,
    MOVE,
    STOP,
    WAIT_1_S
}; 

ChickenHatchStates state;
HatchRequestAction currentHighestPrioritizedHatchRequest;
ActuatorAction currentActuatorActionToPerform;
unsigned long hatchStopChrono;

void setup() {
    Serial.begin(SETTINGS_DATA_SERIAL_MONITOR_BAUD); // initialize serial monitor with 115200 baud
    delay(20);
    distanceMeter.initialize();
    delay(500);            // And wait for half a second.
    actualHatchLocomotion = HatchLocomotion::NO_CHANGE;
    previousHatchLocomotion = HatchLocomotion::NO_CHANGE;

    onboardLED.initialize();

    blinker.initialize();
    blinker.start();
     
    actuator.initialize();
    actuatorPullButton.initialize();
    actuatorPushButton.initialize();

    switchesManager.initialize();

    wifiManager.initialize();
    webserver.initialize();
    mQTTC.initialize();

    state = ChickenHatchStates::STAND_BY;

}

bool requestsForSameLevel() {
    // this should be determined with LIDARS
    return false;
}

bool requestsForOtherLevel() {

    const bool buttonRequestActionIsNotStop = hatchRequests[0].getAction() != HatchRequestAction::STOP;
    const bool buttonRequestIsNotAcknowledged = !hatchRequests[0].getAcknowledged();

    const bool buttonRequestActionIsNotStopAndIsNotAcknowledged = buttonRequestActionIsNotStop && buttonRequestIsNotAcknowledged;


    const bool httpRequestActionIsNotStop = hatchRequests[1].getAction() != HatchRequestAction::STOP;
    const bool httpRequestIsNotAcknowledged = !hatchRequests[1].getAcknowledged();

    const bool httpRequestActionIsNotStopAndIsNotAcknowledged = httpRequestActionIsNotStop && httpRequestIsNotAcknowledged;

    if (buttonRequestActionIsNotStopAndIsNotAcknowledged || httpRequestActionIsNotStopAndIsNotAcknowledged) {

        return true;
    }

    return false;

}

HatchRequestAction getPrioritizedHatchRequestAction() {

    /*
    if (actionRequests[1].getAction() == ActuatorAction::TURN_OFF) { // turn off, http-request most important
        return actionRequests[1].getAction();
    }*/
    if (!hatchRequests[0].getAcknowledged()) { // 1st important (button request)
        return hatchRequests[0].getAction();
    }
    if (!hatchRequests[1].getAcknowledged()) { // 2nd important (http requeest)
        return hatchRequests[1].getAction();
    }
}

bool stopRequested() {
    // this function exist for safety, any release of real button/html button will generate a stop request which always should be most important

    // we should have lidar logic here as well

    if (!hatchRequests[0].getAcknowledged() && (hatchRequests[0].getAction() == HatchRequestAction::STOP)) { // 1st important (button request)
        return true;
    }

    if (!hatchRequests[1].getAcknowledged() && (hatchRequests[1].getAction() == HatchRequestAction::STOP)) { // 2nd important (http requeest)
        return true;
    }
    return false;
}

ActuatorAction acknowledgeAllCurrentHacthRequests() {

    hatchRequests[0].setAcknowledged(true);
    hatchRequests[1].setAcknowledged(true);
}

ActuatorAction hatchAction2ActuatorAction(HatchRequestAction hatchAction) {
    ActuatorAction actuatorAction;

    switch (hatchAction) {
        case HatchRequestAction::UP:
            actuatorAction = ACTUATOR_PULL_WILL_OPEN_HATCH ? ActuatorAction::PULL : ActuatorAction::PUSH;
            break;
        case HatchRequestAction::STOP:
            actuatorAction = ActuatorAction::TURN_OFF;
            break;
        case HatchRequestAction::DOWN:
            actuatorAction = ACTUATOR_PULL_WILL_OPEN_HATCH ? ActuatorAction::PUSH : ActuatorAction::PULL;
            break;
        case HatchRequestAction::CLOSE_HATCH:
            break;
        case HatchRequestAction::OPEN_HATCH:
            break;
    }
    return actuatorAction;
}

void PerformCurrentActuatorAction() {

    switch (currentActuatorActionToPerform) {

        case ActuatorAction::TURN_OFF:
            actuator.turnOFF();
            break;

        case ActuatorAction::PULL:
            actuator.pull();
            break;

        case ActuatorAction::PUSH:
            actuator.push();
            break;
    }
}

void ChickenHatchStateMachine() {

    switch (state)
    {

        case ChickenHatchStates::STAND_BY:

            currentActuatorActionToPerform = ActuatorAction::TURN_OFF;
            
            wifiManager.isConnectedToWifi() ? blinker.setBlinkerSequence(Blinker::BlinkerSequence::STAND_BY) : blinker.setBlinkerSequence(Blinker::BlinkerSequence::STAND_BY_NON_WIFI);

            if (requestsForSameLevel()) {
                state = ChickenHatchStates::STOP;
            }
                      
            else if (requestsForOtherLevel()) {
                currentHighestPrioritizedHatchRequest = getPrioritizedHatchRequestAction();
                acknowledgeAllCurrentHacthRequests();

                currentActuatorActionToPerform = hatchAction2ActuatorAction(currentHighestPrioritizedHatchRequest);
                state = ChickenHatchStates::MOVE;
            }
            break;

        case ChickenHatchStates::MOVE:
            blinker.setBlinkerSequence(Blinker::BlinkerSequence::PERFORMING_MOVE);

            if (stopRequested()) {
                acknowledgeAllCurrentHacthRequests();

                state = ChickenHatchStates::STOP;
            }

            break;

        case ChickenHatchStates::STOP:

            currentActuatorActionToPerform = ActuatorAction::TURN_OFF;

            state = ChickenHatchStates::WAIT_1_S;
            hatchStopChrono = millis();

            break;
        
        case ChickenHatchStates::WAIT_1_S:

            if (millis() - hatchStopChrono > 1000) {
                state = ChickenHatchStates::STAND_BY;
            }

            break;

    }   
}

void indicateMoving() {

    if (currentMeterData.distanceToObjectCm > previousMeterData.distanceToObjectCm) {
        actualHatchLocomotion = HatchLocomotion::DOWN;
    } else if (currentMeterData.distanceToObjectCm < previousMeterData.distanceToObjectCm) {        
        actualHatchLocomotion = HatchLocomotion::UP;
    } else {
        actualHatchLocomotion = HatchLocomotion::NO_CHANGE;
    }
    if (actualHatchLocomotion != previousHatchLocomotion) {

        mQTTC.reportHen_HouseHatchLidar(currentMeterData); 
    }   
    actualHatchLocomotion = previousHatchLocomotion;
};

void loop() {
    switchesManager.monitorInteractions();
    wifiManager.monitorWiFi();
    webserver.handleClient();

    ChickenHatchStateMachine();

    if (wifiManager.isConnectedToWifi()) {
        if (!mQTTC.isConnectedToMQTTBroker()) {
            mQTTC.connectToMQTTBroker();
        }
        pubSubClient.loop();
    }

    PerformCurrentActuatorAction();

    blinker.handleBlinker();

    previousMeterData = currentMeterData;
    distanceMeter.handleDistanceMeter();
    indicateMoving();
}