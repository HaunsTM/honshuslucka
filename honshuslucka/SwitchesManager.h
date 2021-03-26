#include "HatchRequest.h"
#include "Switch.h"

#ifndef SWITCHES_MANAGER_H
#define SWITCHES_MANAGER_H

class SwitchesManager {

private:
    HatchRequest& _hatchRequest;
    Switch& _actuatorPullButton;
    Switch& _actuatorPushButton;

    bool _pullShouldHaveAcknowledgedReset;
    bool _turnOffShouldHaveAcknowledgedReset;
    bool _pushShouldHaveAcknowledgedReset;

    unsigned long _lastAcknowledgementChangedMs;
    const unsigned int MIN_TIME_DURATION_BETWEEN_LAST_ACKNOWLEDGED_TURN_OFF_AND_NEW_ACTION_MS = 10;

    bool newButtonInteractionMayBePerformed();

    bool _initialized;

public:
    SwitchesManager(HatchRequest& hatchRequest, Switch& actuatorPullButton, Switch& actuatorPushButton);

    void initialize();
    void monitorInteractions();
};

#endif