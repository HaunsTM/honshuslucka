#include <Arduino.h>
#include "TextMessageGenerator.h"

#include <Vector.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Actuator.h"
#include "Blinker.h"
#include "ActuatorAction.h"
#include "OnboardLED.h"
#include "Switch.h"
#include "SwitchesManager.h"
#include "Pins.h"
#include "SettingsData.h"
#include "HatchRequest.h"
#include "WifiManager.h"
#include "HTTPWebServer.h"

#include "TextMessageGenerator.h"
#include "MQTTCommunicator.h"


OnboardLED onboardLED;
Blinker blinker(onboardLED);

Actuator actuator(PIN_D6_GPIO12_MISO, PIN_D7_GPIO13_MOSI);

Switch actuatorPullButton(PIN_D2_GPIO4_SDA, true);
Switch actuatorPushButton(PIN_D3_GPIO0_FLASH, true);

TextMessageGenerator tMG(
    SETTINGS_DATA_FIRMWARE_VERSION, SETTINGS_DATA_SERIAL_MONITOR_BAUD,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD,
    SETTINGS_DATA_MQTT_TOPIC_SUBSCRIBE, SETTINGS_DATA_MQTT_TOPIC_PUBLISH);

int nextBlinkStepToPerform = 0;
ActuatorAction m;

HatchRequest hatchRequests[2] = { HatchRequest("switch button requested"), HatchRequest("http requested") };

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);
ESP8266WebServer eSP8266WebServer(SETTINGS_DATA_WEB_SERVER_PORT);

void initiateRequestArray() {
    hatchRequests[0].initialize();
    hatchRequests[1].initialize();
}

SwitchesManager switchesManager(hatchRequests[0], actuatorPullButton, actuatorPushButton);

WifiManager wifiManager(
    tMG,
    CREDENTIALS_JSON_STRING);

HTTPWebServer webserver(hatchRequests[1], eSP8266WebServer, tMG);

MQTTCommunicator mQTTC(
    pubSubClient, m, tMG,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD,
    SETTINGS_DATA_MQTT_TOPIC_SUBSCRIBE);

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

    onboardLED.initialize();

    blinker.initialize();
     
    actuator.initialize();
    actuatorPullButton.initialize();
    actuatorPushButton.initialize();

    switchesManager.initialize();

    wifiManager.initialize();
    webserver.initialize();
    mQTTC.initialize();

    state = ChickenHatchStates::STAND_BY;

    Serial.println("1");
    /*
    nextBlinkStepToPerform = 0;
    m = ActuatorAction::turnOff;
    actuator.turnOFF();
    */
}

bool requestsForSameLevel() {
    // this should be determined with LIDARS
    return false;
}

bool requestsForOtherLevel() {

    const bool buttonRequestIsAcknowledged = hatchRequests[0].getAcknowledged();
    const bool httpRequestIsAcknowledged = hatchRequests[1].getAcknowledged();

    const bool anyActionRequestIsNotAcknowledged = !(buttonRequestIsAcknowledged && httpRequestIsAcknowledged);

    return anyActionRequestIsNotAcknowledged;
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

    if (!hatchRequests[0].getAcknowledged() && (hatchRequests[0].getAction() == HatchRequestAction::STOP)) { // 1st important (button request)
        return true;
    }

    if (!hatchRequests[1].getAcknowledged() && (hatchRequests[0].getAction() == HatchRequestAction::STOP)) { // 2nd important (http requeest)
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
    actuatorAction;
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

            if (requestsForSameLevel()) {

                state = ChickenHatchStates::STOP;
            }
            else if (requestsForOtherLevel()) {

                Serial.println("C");
                currentHighestPrioritizedHatchRequest = getPrioritizedHatchRequestAction();
                acknowledgeAllCurrentHacthRequests();
                currentActuatorActionToPerform = hatchAction2ActuatorAction(currentHighestPrioritizedHatchRequest);

                state = ChickenHatchStates::MOVE;
            }
            break;
        case ChickenHatchStates::MOVE:

            if (stopRequested()) {

                Serial.println("D");
                state = ChickenHatchStates::MOVE;
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

            //Serial.println("F");
            if (millis() - hatchStopChrono > 1000) {
                state = ChickenHatchStates::STAND_BY;

                Serial.println("G");
            }
            break;

        default:

            Serial.println("F");
            break;
    }   
}

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
 /*
    while (anyButtonIsPressed())
    {
        m = ActuatorAction::turnOff;
        blinker.setSequencePerformingAction();
        while (actuatorPullButton.isPressed())
        {
            actuator.pull();

            if (!blinker.blinkSequenceCompleted()) {
                nextBlinkStepToPerform = blinker.performBlinkSequenceStep(nextBlinkStepToPerform);
            }
            else {
                nextBlinkStepToPerform = blinker.setSequencePerformingAction();
            }
            yield();
        }

        while (actuatorPushButton.isPressed())
        {
            actuator.push();

            if (!blinker.blinkSequenceCompleted()) {
                nextBlinkStepToPerform = blinker.performBlinkSequenceStep(nextBlinkStepToPerform);
            }
            else {
                nextBlinkStepToPerform = blinker.setSequencePerformingAction();
            }
            yield();
        }

        nextBlinkStepToPerform = blinker.setSequenceStandByNoWifi();
        actuator.turnOFF();
        delay(10);
    }
    
    if (!anyButtonIsPressed())
    {
        switch (m) {
            case ActuatorAction::pull:
                actuator.pull();
                break;
            case ActuatorAction::turnOff:
                actuator.turnOFF();
                break;
            case ActuatorAction::push:
                actuator.push();
                break;
        }
    }   
    
    if (!blinker.blinkSequenceCompleted()) {
        nextBlinkStepToPerform = blinker.performBlinkSequenceStep(nextBlinkStepToPerform);
    }
    else {
        nextBlinkStepToPerform = blinker.setSequenceStandByNoWifi();
    }

    delay(10);

    */
}