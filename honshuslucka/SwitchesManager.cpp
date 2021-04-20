#include <Arduino.h>
#include "HatchRequest.h"
#include "HatchRequestAction.h"
#include "Switch.h"
#include "SwitchesManager.h"

SwitchesManager::SwitchesManager(HatchRequest& hatchRequest, Switch& actuatorPullButton, Switch& actuatorPushButton) :
    _hatchRequest(hatchRequest),
    _actuatorPullButton(actuatorPullButton),
    _actuatorPushButton(actuatorPushButton)
{
    _lastAcknowledgementChangedMs = 0;

    _initialized = false;
};

void SwitchesManager::initialize() {
    if (!_initialized) {

        _pullShouldHaveAcknowledgedReset = true;
        _turnOffShouldHaveAcknowledgedReset = false;
        _pushShouldHaveAcknowledgedReset = true;

        _hatchRequest.setAction(HatchRequestAction::STOP);
        _hatchRequest.setAcknowledged(true);
        _initialized = true;
    }
};

bool SwitchesManager::newButtonInteractionMayBePerformed() {
    return true;
    unsigned long currentTimeMs = millis();

    if (currentTimeMs < _lastAcknowledgementChangedMs) {
        // special case - we have rounded max time
        return true;
    }
    const bool timeElapsedSinceLatestAction = currentTimeMs > (_lastAcknowledgementChangedMs + MIN_TIME_DURATION_BETWEEN_LAST_ACKNOWLEDGED_TURN_OFF_AND_NEW_ACTION_MS);
    const bool latestActionWasTurnOff = _hatchRequest.getAction() == HatchRequestAction::STOP;

    const bool newButtonInteractionMayBePerformed = timeElapsedSinceLatestAction && latestActionWasTurnOff;

    return newButtonInteractionMayBePerformed;
}

void SwitchesManager::monitorInteractions() {

    bool actuatorPullButtonIsPressed = _actuatorPullButton.isPressed();
    bool actuatorPushButtonIsPressed = _actuatorPushButton.isPressed();

    if (actuatorPullButtonIsPressed && newButtonInteractionMayBePerformed()) {
        _hatchRequest.setAction(HatchRequestAction::UP);

        if (_pullShouldHaveAcknowledgedReset) {
            _hatchRequest.setAcknowledged(false);
            _lastAcknowledgementChangedMs = millis();

            _pullShouldHaveAcknowledgedReset = false;
            _turnOffShouldHaveAcknowledgedReset = true;
        }
         
    }

    else if (!actuatorPullButtonIsPressed && !actuatorPushButtonIsPressed) {
        _hatchRequest.setAction(HatchRequestAction::STOP);

        if (_turnOffShouldHaveAcknowledgedReset && newButtonInteractionMayBePerformed()) {
            _hatchRequest.setAcknowledged(false);
            _lastAcknowledgementChangedMs = millis();

            _pullShouldHaveAcknowledgedReset = true;
            _turnOffShouldHaveAcknowledgedReset = false;
            _pushShouldHaveAcknowledgedReset = true;
        }
    }

    else if (actuatorPushButtonIsPressed && newButtonInteractionMayBePerformed()) {
        _hatchRequest.setAction(HatchRequestAction::DOWN);

        if (_pushShouldHaveAcknowledgedReset) {
            _hatchRequest.setAcknowledged(false);
            _lastAcknowledgementChangedMs = millis();

            _pushShouldHaveAcknowledgedReset = false;
            _turnOffShouldHaveAcknowledgedReset = true;
        }
    }
}