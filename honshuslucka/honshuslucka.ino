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
#include "ActionRequest.h"
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

ActionRequest actionRequests[3] = { ActionRequest("switch button requested"), ActionRequest("http requested"), ActionRequest("mqtt requested") };

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);
ESP8266WebServer eSP8266WebServer(SETTINGS_DATA_WEB_SERVER_PORT);

void initiateRequestArray() {
    actionRequests[0].initialize();
    actionRequests[1].initialize();
    actionRequests[2].initialize();
}

SwitchesManager switchesManager(actionRequests[0], actuatorPullButton, actuatorPushButton);

WifiManager wifiManager(
    tMG,
    CREDENTIALS_JSON_STRING);

HTTPWebServer webserver(actionRequests[1], eSP8266WebServer, tMG);

MQTTCommunicator mQTTC(
    pubSubClient, m, tMG,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD,
    SETTINGS_DATA_MQTT_TOPIC_SUBSCRIBE);

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

    /*
    nextBlinkStepToPerform = 0;
    m = ActuatorAction::turnOff;
    actuator.turnOFF();
    */
}

/*
bool anyButtonIsPressed() {
    bool anyIsPressed = actuatorPullButton.isPressed() || actuatorPushButton.isPressed();
    return anyIsPressed;
}
*/

void performAction() {

    if (actionRequests[0].getAction() == ActuatorAction::PULL && !actionRequests[0].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, LOW);
        actuator.pull();
        Serial.print("1 - ");
        actionRequests[0].setAcknowledged(true);
    } else if (actionRequests[0].getAction() == ActuatorAction::TURN_OFF && !actionRequests[0].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, HIGH);
        actuator.turnOFF();
        Serial.print("2 - ");
        actionRequests[0].setAcknowledged(true);
    } else if (actionRequests[0].getAction() == ActuatorAction::PUSH && !actionRequests[0].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, LOW);

        actuator.push();
        Serial.print("3 - ");
        actionRequests[0].setAcknowledged(true);
    } else if (actionRequests[1].getAction() == ActuatorAction::PULL && !actionRequests[1].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, LOW);
        actuator.pull();
        Serial.print("4 - ");
        actionRequests[1].setAcknowledged(true);
    } else if (actionRequests[1].getAction() == ActuatorAction::TURN_OFF && !actionRequests[1].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, HIGH);
        actuator.turnOFF();
        Serial.print("5 - ");
        actionRequests[1].setAcknowledged(true);
    } else if (actionRequests[1].getAction() == ActuatorAction::PUSH && !actionRequests[1].getAcknowledged()) {
        digitalWrite(LED_BUILTIN, LOW);

        actuator.push();
        Serial.print("6 - ");
        actionRequests[1].setAcknowledged(true);
    };
}

void loop() {
    switchesManager.monitorInteractions();
    wifiManager.monitorWiFi();
    webserver.handleClient();

    performAction();
   

    if (wifiManager.isConnectedToWifi()) {
        if (!mQTTC.isConnectedToMQTTBroker()) {
            mQTTC.connectToMQTTBroker();
        }
        pubSubClient.loop();
    }
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