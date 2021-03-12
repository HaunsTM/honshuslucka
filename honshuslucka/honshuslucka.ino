#include <Arduino.h>

#include "TextMessageGenerator.h"

#include <Vector.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Actuator.h"
#include "Blinker.h"
#include "MqttAction.h"
#include "OnboardLED.h"
#include "Switch.h"
#include "Pins.h"
#include "SecretData.h"
#include "WifiManager.h"

#include "TextMessageGenerator.h"
#include "MQTTCommunicator.h"


OnboardLED onboardLED;

Actuator actuator(PIN_D6_GPIO12_MISO, PIN_D7_GPIO13_MOSI);
Blinker blinker(onboardLED);
Switch actuatorPullButton(PIN_D2_GPIO4_SDA, true);
Switch actuatorPushButton(PIN_D3_GPIO0_FLASH, true);

TextMessageGenerator tMG(
    SECRET_DATA_MQTT_BROKER_URL, SECRET_DATA_MQTT_PORT,
    SECRET_DATA_MQTT_USERNAME, SECRET_DATA_MQTT_PASSWORD,
    SECRET_DATA_MQTT_TOPIC_SUBSCRIBE, SECRET_DATA_MQTT_TOPIC_PUBLISH);

int nextBlinkStepToPerform = 0;
MqttAction m;

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

WifiManager wifiManager(
    tMG,
    CREDENTIALS_JSON_STRING);

MQTTCommunicator mQTTC(
    pubSubClient, m, tMG,
    SECRET_DATA_MQTT_BROKER_URL, SECRET_DATA_MQTT_PORT,
    SECRET_DATA_MQTT_USERNAME, SECRET_DATA_MQTT_PASSWORD,
    SECRET_DATA_MQTT_TOPIC_SUBSCRIBE);

void setup() {
  
    Serial.begin(115200); // initialize serial monitor with 9600 baud

    onboardLED.initialize();

    blinker.initialize();
     
    actuator.initialize();
    actuatorPullButton.initialize();
    actuatorPushButton.initialize();

    wifiManager.initialize();
    mQTTC.initialize();

    nextBlinkStepToPerform = 0;
    m = MqttAction::turnOff;
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

    if (wifiManager.isConnectedToWifi()) {
        if (!mQTTC.isConnectedToMQTTBroker()) {
            mQTTC.connectToMQTTBroker();
        }
        pubSubClient.loop();
    }

    while (anyButtonIsPressed())
    {
        m = MqttAction::turnOff;
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
            case MqttAction::pull:
                actuator.pull();
                break;
            case MqttAction::turnOff:
                actuator.turnOFF();
                break;
            case MqttAction::push:
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
