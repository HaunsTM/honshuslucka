#include <string>
#include "HatchRequestAction.h"
#include "HatchRequest.h"

HatchRequest::HatchRequest(std::string name) {

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

std::string HatchRequest::getName() {
	return _name;
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