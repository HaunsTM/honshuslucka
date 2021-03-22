#include "ActionRequest.h"
#include "Switch.h"

#ifndef SWITCHES_MANAGER_H
#define SWITCHES_MANAGER_H

class SwitchesManager {

private:
    ActionRequest& _actionRequest;
    Switch& _actuatorPullButton;
    Switch& _actuatorPushButton;

    bool _actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed;

    bool _initialized;

public:
    SwitchesManager(ActionRequest& actionRequest, Switch& actuatorPullButton, Switch& actuatorPushButton);

    void initialize();
    void monitorInteractions();
};

#endif