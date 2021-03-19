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
#include "Pins.h"
#include "SettingsData.h"
#include "WifiManager.h"
#include "HTTPWebServer.h"

#include "TextMessageGenerator.h"
#include "MQTTCommunicator.h"


OnboardLED onboardLED;

Actuator actuator(PIN_D6_GPIO12_MISO, PIN_D7_GPIO13_MOSI);
Blinker blinker(onboardLED);
Switch actuatorPullButton(PIN_D2_GPIO4_SDA, true);
Switch actuatorPushButton(PIN_D3_GPIO0_FLASH, true);

TextMessageGenerator tMG(
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD,
    SETTINGS_DATA_MQTT_TOPIC_SUBSCRIBE, SETTINGS_DATA_MQTT_TOPIC_PUBLISH);

int nextBlinkStepToPerform = 0;
ActuatorAction m;

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);
ESP8266WebServer eSP8266WebServer(SETTINGS_DATA_WEB_SERVER_PORT);

WifiManager wifiManager(
    tMG,
    CREDENTIALS_JSON_STRING);

HTTPWebServer webserver(eSP8266WebServer, tMG);

MQTTCommunicator mQTTC(
    pubSubClient, m, tMG,
    SETTINGS_DATA_MQTT_BROKER_URL, SETTINGS_DATA_MQTT_PORT,
    SETTINGS_DATA_MQTT_USERNAME, SETTINGS_DATA_MQTT_PASSWORD,
    SETTINGS_DATA_MQTT_TOPIC_SUBSCRIBE);

void setup() {
    Serial.begin(115200); // initialize serial monitor with 9600 baud

    onboardLED.initialize();

    blinker.initialize();
     
    actuator.initialize();
    actuatorPullButton.initialize();
    actuatorPushButton.initialize();

    wifiManager.initialize();
    webserver.initialize();
    mQTTC.initialize();

    nextBlinkStepToPerform = 0;
    m = ActuatorAction::turnOff;
    actuator.turnOFF();
}

bool isMoving() {
    bool isMoving = false;
    return isMoving;
}

bool anyButtonIsPressed() {
    bool anyIsPressed = actuatorPullButton.isPressed() || actuatorPushButton.isPressed();
    return anyIsPressed;
}

void loop() {
    wifiManager.monitorWiFi();
    webserver.handleClient();

    if (wifiManager.isConnectedToWifi()) {
        if (!mQTTC.isConnectedToMQTTBroker()) {
            mQTTC.connectToMQTTBroker();
        }
        pubSubClient.loop();
    }

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
}