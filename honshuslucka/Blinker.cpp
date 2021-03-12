#include <Arduino.h>
#include "Blinker.h"
#include "OnboardLED.h"
#include "StateAndDuration.h"
#include <Vector.h>

Blinker::Blinker(OnboardLED& onboardLED) {
    _onboardLED = onboardLED;
    _initialized = false;
}

int Blinker::initialSequenceStepToPerform() {
    _previousMillis = 0;
    _lastPerformedStep = INITIAL_STEP_TO_PERFORM;
    return INITIAL_STEP_TO_PERFORM;
}

void Blinker::initialize() {

    if (!_initialized) {

        _blinkSequenceList.setStorage(_blinkSequenceArray);

        _onboardLED.setOff();
        _initialized = true;
    }
}

int Blinker::setErrorState() {
    _blinkSequenceList.clear();

    StateAndDuration ledOn(OnboardLED::LEDState::ON, 50);
    StateAndDuration ledOffShort(OnboardLED::LEDState::OFF, 50);
    StateAndDuration ledOff(OnboardLED::LEDState::OFF, 750);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOff);

    return initialSequenceStepToPerform();
};

int Blinker::setSequenceHttpRequest() {
    _blinkSequenceList.clear();

    StateAndDuration ledOn(OnboardLED::LEDState::ON, 50);
    StateAndDuration ledOffShort(OnboardLED::LEDState::OFF, 50);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);

    return initialSequenceStepToPerform();
};

int Blinker::setSequenceMqttArrived() {
    int cloneOfSetSequenceHttpRequest = setSequenceHttpRequest();

    return initialSequenceStepToPerform();
};

int Blinker::setSequencePerformingAction() {
    _blinkSequenceList.clear();

    StateAndDuration ledOn(OnboardLED::LEDState::ON, 50);
    StateAndDuration ledOffLong(OnboardLED::LEDState::OFF, 450);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffLong);

    return initialSequenceStepToPerform();
};

int Blinker::setSequenceStandBy() {
    _blinkSequenceList.clear();

    StateAndDuration ledOn(OnboardLED::LEDState::ON, 50);
    StateAndDuration ledOff(OnboardLED::LEDState::OFF, 4950);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOff);

    return initialSequenceStepToPerform();
};

int Blinker::setSequenceStandByNoWifi() {
    _blinkSequenceList.clear();

    StateAndDuration ledOn(OnboardLED::LEDState::ON, 50);
    StateAndDuration ledOffShort(OnboardLED::LEDState::OFF, 50);
    StateAndDuration ledOffLong(OnboardLED::LEDState::OFF, 4750);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffShort);
    _blinkSequenceList.push_back(ledOn);
    _blinkSequenceList.push_back(ledOffLong);

    return initialSequenceStepToPerform();
};

void Blinker::set(OnboardLED::LEDState state) {
    String s;
    char buf[12];
    switch (state) {
        case OnboardLED::LEDState::ON:
            s += String("LED - on :  ") + itoa(millis(), buf, 10) + String("\n");
            _onboardLED.setOn();
            break;
        case OnboardLED::LEDState::OFF:
            s += String("LED - off :  ") + itoa(millis(), buf, 10) + String("\n");
            _onboardLED.setOff();
            break;
    }
     // Serial.println(s.c_str());
};

bool Blinker::blinkSequenceCompleted() {   

    char buf[12];
    String s;

    int blinkSequenceListSize = _blinkSequenceList.size();

    s += String("-------------------------------")  + String("\n");
    s += String("blinkSequenceListSize: ") + itoa(blinkSequenceListSize, buf, 10) + String("\n");

    bool blinkSequenceNotCompleted;

    if (blinkSequenceListSize > 0) {
        int maxStepIndex = blinkSequenceListSize - 1;
        blinkSequenceNotCompleted = _lastPerformedStep <= maxStepIndex;
        s += String("maxStepIndex: ") + itoa(maxStepIndex, buf, 10) + String("\n");
        s += String("_lastPerformedStep: ") + itoa(_lastPerformedStep, buf, 10) + String("\n");
        s += String("blinkSequenceNotCompleted: ") + (blinkSequenceNotCompleted?"true":"false") + String("\n");
        //Serial.println(s.c_str());

        return !blinkSequenceNotCompleted;
    }; 
    s += String("blinkSequenceCompleted: ") + String("true") + String("\n");
    // Serial.println(s.c_str());
    return true;
}

int Blinker::performBlinkSequenceStep(int stepToPossiblyMoveOnTo) {

    String s;
    char buf[12];

    s += String("-------------- Blinker::performBlinkSequenceStep() --------------\n");
    unsigned long currentMillis = millis();
    int nextStep = stepToPossiblyMoveOnTo;
    int finalStepToPerform = _blinkSequenceList.size() - 1;

    OnboardLED::LEDState ledStateForNextStep = _blinkSequenceList.at(stepToPossiblyMoveOnTo).ledState();
    bool performStep = false;


    if (stepToPossiblyMoveOnTo == INITIAL_STEP_TO_PERFORM) {
        performStep = true;
    }
    else {
        int previousStep = stepToPossiblyMoveOnTo - 1;
        unsigned int interval = _blinkSequenceList.at(previousStep).stateLengthMs();
        unsigned long timeWhenNextStepCanBePerformedEarliest = _previousMillis + interval;
        performStep = currentMillis >= timeWhenNextStepCanBePerformedEarliest;

        s += String("stepToPossiblyMoveOnTo:  ") + itoa(stepToPossiblyMoveOnTo, buf, 10) + String("\n");
        s += String("interval: ") + itoa(interval, buf, 10) + String("\n");
        s += String("_previousMillis: ") + itoa(_previousMillis, buf, 10) + String("\n");
        s += String("currentMillis: ") + itoa(currentMillis, buf, 10) + String("\n");
        s += String("timeWhenNextStepCanBePerformedEarliest: ") + itoa(timeWhenNextStepCanBePerformedEarliest, buf, 10) + String("\n");
        s += String("performStep: ") + (performStep?"true":"false") + String("\n");
    }

    if (performStep) {
        _previousMillis = currentMillis;
        set(ledStateForNextStep);
        _lastPerformedStep = stepToPossiblyMoveOnTo;
        nextStep = stepToPossiblyMoveOnTo + 1;
    }
    s += String("\n");
    // Serial.println(s.c_str());
    return nextStep;
};