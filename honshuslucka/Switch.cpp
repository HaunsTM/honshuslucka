#include <Arduino.h>
#include "Switch.h"

Switch::Switch(int pinPushButton, bool normallyOpen ) {
    _pinSwitch = pinPushButton;
    _normallyOpen = normallyOpen;
    _initialized = false;
}

void Switch::initialize() {
    if (!_initialized) {
        pinMode(_pinSwitch, INPUT_PULLUP);

        _initialized = true;
    }
}

bool Switch::isPressed() {
    if (_normallyOpen) {
        return digitalRead(_pinSwitch) == LOW;
    }
    return digitalRead(_pinSwitch) == HIGH;
}

int Switch::getPinSwitch() {
    return _pinSwitch;
};

bool Switch::getNormallyOpen() {
    return _normallyOpen;
};