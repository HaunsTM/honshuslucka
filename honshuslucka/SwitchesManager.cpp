#include <Arduino.h>
#include "ActionRequest.h"
#include "Switch.h"
#include "SwitchesManager.h"

SwitchesManager::SwitchesManager(ActionRequest& actionRequest, Switch& actuatorPullButton, Switch& actuatorPushButton)     :
    _actionRequest(actionRequest),
    _actuatorPullButton(actuatorPullButton),
    _actuatorPushButton(actuatorPushButton)
{

    _initialized = false;
};

void SwitchesManager::initialize() {
    if (!_initialized) {
        _actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed = true;
        _actionRequest.setAction(ActuatorAction::TURN_OFF);
        _actionRequest.setAcknowledged(true);
        _initialized = true;
    }
};

void SwitchesManager::monitorInteractions() {
    bool actuatorPullButtonIsPressed = _actuatorPullButton.isPressed();
    bool actuatorPushButtonIsPressed = _actuatorPushButton.isPressed();

    if (actuatorPullButtonIsPressed) {
        _actionRequest.setAcknowledged(false);
        _actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed = true;
        _actionRequest.setAction(ActuatorAction::PULL);
    }
    else if (actuatorPushButtonIsPressed) {
        _actionRequest.setAcknowledged(false);
        _actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed = true;
        _actionRequest.setAction(ActuatorAction::PUSH);
    }
    else {
        _actionRequest.setAction(ActuatorAction::TURN_OFF);
        if (_actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed) {
            _actuatorTurnOffShouldBeSetNotAcknowledgedNextTimeMonitorIsInvokedWhenNoButtonIsPressed = false;
            _actionRequest.setAcknowledged(false);
        }
    }
}