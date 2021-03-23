#include "ActionRequest.h"
#include "Switch.h"

#ifndef SWITCHES_MANAGER_H
#define SWITCHES_MANAGER_H

class SwitchesManager {

private:
    ActionRequest& _actionRequest;
    Switch& _actuatorPullButton;
    Switch& _actuatorPushButton;

    bool _pullShouldHaveAcknowledgedReset;
    bool _turnOffShouldHaveAcknowledgedReset;
    bool _pushShouldHaveAcknowledgedReset;

    unsigned long _lastAcknowledgementChangedMs;
    const unsigned int MIN_TIME_DURATION_BETWEEN_LAST_ACKNOWLEDGED_TURN_OFF_AND_NEW_ACTION_MS = 500;

    bool newButtonInteractionMayBePerformed();

    bool _initialized;

public:
    SwitchesManager(ActionRequest& actionRequest, Switch& actuatorPullButton, Switch& actuatorPushButton);

    void initialize();
    void monitorInteractions();
};

#endif