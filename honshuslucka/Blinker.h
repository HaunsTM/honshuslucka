#include <Arduino.h>
#include "OnboardLED.h"
#include "StateAndDuration.h"
#include <Vector.h>

#ifndef BLINKER_H
#define BLINKER_H

class Blinker {

public:
    Blinker(OnboardLED& onboardLED);

    void initialize();

    int setErrorState();
    int setSequenceHttpRequest();
    int setSequenceMqttArrived();
    int setSequencePerformingAction();
    int setSequenceStandBy();
    int setSequenceStandByNoWifi();

    int performBlinkSequenceStep(int stepToPerform);
    bool blinkSequenceCompleted();

    void set(OnboardLED::LEDState state);

private:
    static const int ELEMENT_COUNT_MAX = 10;
    static const int INITIAL_STEP_TO_PERFORM = 0;
    OnboardLED _onboardLED;
    unsigned long _previousMillis;
    unsigned long _lastPerformedStep;

    int initialSequenceStepToPerform();

    typedef Vector<StateAndDuration> Elements;
    StateAndDuration _blinkSequenceArray[ELEMENT_COUNT_MAX];
    Elements _blinkSequenceList;

    bool _initialized;
};

#endif