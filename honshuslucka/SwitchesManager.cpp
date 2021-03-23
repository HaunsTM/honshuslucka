#include <Arduino.h>
#include "ActionRequest.h"
#include "Switch.h"
#include "SwitchesManager.h"

SwitchesManager::SwitchesManager(ActionRequest& actionRequest, Switch& actuatorPullButton, Switch& actuatorPushButton) :
    _actionRequest(actionRequest),
    _actuatorPullButton(actuatorPullButton),
    _actuatorPushButton(actuatorPushButton)
{
    _lastAcknowledgementChangedMs = 0;

    _initialized = false;
};

void SwitchesManager::initialize() {
    if (!_initialized) {

        _pullShouldHaveAcknowledgedReset = true;
        _turnOffShouldHaveAcknowledgedReset = true;
        _pushShouldHaveAcknowledgedReset = true;

        _actionRequest.setAction(ActuatorAction::TURN_OFF);
        _actionRequest.setAcknowledged(true);
        _initialized = true;
    }
};

bool SwitchesManager::newButtonInteractionMayBePerformed() {
    unsigned long currentTimeMs = millis();

    if (currentTimeMs < _lastAcknowledgementChangedMs) {
        // special case - we have rounded max time
        return true;
    }
    const bool timeElapsedSinceLatestAction = currentTimeMs > (_lastAcknowledgementChangedMs + MIN_TIME_DURATION_BETWEEN_LAST_ACKNOWLEDGED_TURN_OFF_AND_NEW_ACTION_MS);
    const bool latestActionWasTurnOff = _actionRequest.getAction() == ActuatorAction::TURN_OFF;

    const bool newButtonInteractionMayBePerformed = timeElapsedSinceLatestAction && latestActionWasTurnOff;

    return newButtonInteractionMayBePerformed;
}

void SwitchesManager::monitorInteractions() {

    bool actuatorPullButtonIsPressed = _actuatorPullButton.isPressed();
    bool actuatorPushButtonIsPressed = _actuatorPushButton.isPressed();

    if (actuatorPullButtonIsPressed && newButtonInteractionMayBePerformed()) {
        
        _actionRequest.setAction(ActuatorAction::PULL);

        if (_pullShouldHaveAcknowledgedReset) {
            _actionRequest.setAcknowledged(false);
            _lastAcknowledgementChangedMs = millis();

            _pullShouldHaveAcknowledgedReset = false;
            _turnOffShouldHaveAcknowledgedReset = true;
        }
         
    }

    else if (!actuatorPullButtonIsPressed && !actuatorPushButtonIsPressed) {
        
        _actionRequest.setAction(ActuatorAction::TURN_OFF);

        if (_turnOffShouldHaveAcknowledgedReset && newButtonInteractionMayBePerformed()) {
            _actionRequest.setAcknowledged(false); 
            _lastAcknowledgementChangedMs = millis();

            _pullShouldHaveAcknowledgedReset = true;
            _turnOffShouldHaveAcknowledgedReset = false;
            _pushShouldHaveAcknowledgedReset = true;
        }
    }

    else if (actuatorPushButtonIsPressed && newButtonInteractionMayBePerformed()) {

        _actionRequest.setAction(ActuatorAction::PUSH);

        if (_pushShouldHaveAcknowledgedReset) {
            _actionRequest.setAcknowledged(false);
            _lastAcknowledgementChangedMs = millis();

            _pushShouldHaveAcknowledgedReset = false;
            _turnOffShouldHaveAcknowledgedReset = true;
        }
    }
}