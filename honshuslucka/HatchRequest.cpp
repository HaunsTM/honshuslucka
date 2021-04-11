#include <Arduino.h>
#include "HatchRequestAction.h"
#include "HatchRequest.h"

HatchRequest::HatchRequest(String name) {

	_name = name;

	_initialized = false;
};

void HatchRequest::initialize() {
	if (!_initialized) {
		// https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/
		_action = HatchRequestAction::STOP;
		_acknowledged = true;

		_initialized = true;
	}
}

String HatchRequest::getName() {
	return _name;
};

bool HatchRequest::getInitialized() {    
	return _initialized;
};

bool HatchRequest::getAcknowledged() {    
	return _acknowledged;
};

void HatchRequest::setAcknowledged(bool value) {
	_acknowledged = value;
};

HatchRequestAction HatchRequest::getAction() {
	return _action;
};

void HatchRequest::setAction(HatchRequestAction value) {
	_action = value;
};