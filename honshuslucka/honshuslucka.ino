


#include <Arduino.h>
#include "Actuator.h"
#include "ActuatorAction.h"
#include "Blinker.h"
#include "DistanceMeter.h"
#include <ESP8266WiFi.h>
#include "HTTPWebServer.h"
#include "HatchRequest.h"
#include "MQTTCommunicator.h"
#include "OnboardLED.h"
#include "Pins.h"
#include <PubSubClient.h>
#include "SettingsData.h"
#include <SoftwareSerial.h>
#include "Switch.h"
#include "SwitchesManager.h"
#include "TextMessageGenerator.h"
#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.1
#include "WifiManager.h"

TFMPlus tfmP;         // Create a TFMini Plus object  
SoftwareSerial mySerial(PIN_D2_GPIO4_SDA, PIN_D1_GPIO5_SCL); //define software serial port name as mySerial and define D5 as RX and D0 as TX
DistanceMeterData currentMeterData;
DistanceMeterData previousMeterData;
DistanceMeter distanceMeter(mySerial, currentMeterData);

OnboardLED onboardLED;
Blinker blinker(onboardLED);

Actuator actuator(PIN_D6_GPIO12_MISO, PIN_D7_GPIO13_MOSI);

Switch actuatorPullButton(PIN_D3_GPIO0_FLASH, true);
Switch actuatorPushButton(PIN_D5_GPIO14_SCLK, true);

TextMessageGenerator tMG(
    SETTINGS_DATA_FIRMWARE_VERSION, SETTINGS_DATA_SERIAL_MONITOR_BAUD,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT_1883,
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

HTTPWebServer webserver(hatchRequests[1], eSP8266WebServer, tMG, currentMeterData, SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT_FOR_PAHO_1884, SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD);

MQTTCommunicator mQTTC(
    pubSubClient, tMG,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT_1883,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD);

enum class ChickenHatchStates {
    STAND_BY,
    MOVE,
    STOP,
    WAIT_1_S
}; 

ChickenHatchStates state;
HatchRequestAction currentHighestPrioritizedHatchRequestAction;
ActuatorAction currentActuatorActionToPerform;
ActuatorAction previousActuatorAction;
unsigned long hatchMovementStarted;
unsigned long hatchPauseChrono;

void setup() {
    Serial.begin(SETTINGS_DATA_SERIAL_MONITOR_BAUD); // initialize serial monitor with 115200 baud
    delay(20);
    distanceMeter.initialize();
    delay(500);            // And wait for half a second.

    onboardLED.initialize();

    blinker.initialize();
    blinker.start();
     
    actuator.initialize();
    previousActuatorAction = ActuatorAction::TURN_OFF;
    actuatorPullButton.initialize();
    actuatorPushButton.initialize();

    switchesManager.initialize();

    wifiManager.initialize();
    webserver.initialize();
    mQTTC.initialize();

    state = ChickenHatchStates::STAND_BY;

    initiateRequestArray();
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

bool maxHatchMovementTimeReached() {

    unsigned long currentTimeMs = millis();

    if (currentTimeMs < hatchMovementStarted) {
        // special case - we have rounded max time
        return true;
    }

    const bool maxTimeReached = currentTimeMs > (hatchMovementStarted + MAX_REQUIRED_TIME_TO_PERFORM_A_HATCH_OPEN_PROCESS_MS);
    return maxTimeReached;
};

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

void performCurrentActuatorAction() {

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

void reportChangesInCurrentActuatorAction() {

    if (currentActuatorActionToPerform != previousActuatorAction) {
        mQTTC.reportHen_HouseHatchActuatorAction(currentActuatorActionToPerform);
        previousActuatorAction = currentActuatorActionToPerform;
    }
};

void chickenHatchStateMachine() {

    switch (state)
    {

        case ChickenHatchStates::STAND_BY:

            currentActuatorActionToPerform = ActuatorAction::TURN_OFF;
            
            wifiManager.isConnectedToWifi() ? blinker.setBlinkerSequence(Blinker::BlinkerSequence::STAND_BY) : blinker.setBlinkerSequence(Blinker::BlinkerSequence::STAND_BY_NON_WIFI);

            if (requestsForSameLevel()) {
                state = ChickenHatchStates::STOP;
            }
                      
            else if (requestsForOtherLevel()) {
                currentHighestPrioritizedHatchRequestAction = getPrioritizedHatchRequestAction();
                acknowledgeAllCurrentHacthRequests();

                currentActuatorActionToPerform = hatchAction2ActuatorAction(currentHighestPrioritizedHatchRequestAction);
                
                hatchMovementStarted = millis();
                state = ChickenHatchStates::MOVE;
            }
            break;

        case ChickenHatchStates::MOVE:
            blinker.setBlinkerSequence(Blinker::BlinkerSequence::PERFORMING_MOVE);

            if (stopRequested()) {
                acknowledgeAllCurrentHacthRequests();

                state = ChickenHatchStates::STOP;
            }
            
            if (maxHatchMovementTimeReached()) {
                acknowledgeAllCurrentHacthRequests();

                state = ChickenHatchStates::STOP;
            }

            break;

        case ChickenHatchStates::STOP:

            currentActuatorActionToPerform = ActuatorAction::TURN_OFF;

            state = ChickenHatchStates::WAIT_1_S;

            hatchMovementStarted = 0;
            hatchPauseChrono = millis();

            break;
        
        case ChickenHatchStates::WAIT_1_S:

            if (millis() - hatchPauseChrono > 1000) {
                state = ChickenHatchStates::STAND_BY;
            }

            break;

    }   
}

void reportMoving() {

    if (currentMeterData.distanceToObjectCm != previousMeterData.distanceToObjectCm) {
        mQTTC.reportHen_HouseHatchLidar(currentMeterData);
    }

};

void loop() {
    switchesManager.monitorInteractions();
    wifiManager.monitorWiFi();
    webserver.handleClient();

    chickenHatchStateMachine();

    if (wifiManager.isConnectedToWifi()) {
        if (!mQTTC.isConnectedToMQTTBroker()) {
            mQTTC.connectToMQTTBroker();
        }
        pubSubClient.loop();
    }

    performCurrentActuatorAction();
    reportChangesInCurrentActuatorAction();

    blinker.handleBlinker();

    previousMeterData = currentMeterData;
    distanceMeter.handleDistanceMeter();
    reportMoving();
}