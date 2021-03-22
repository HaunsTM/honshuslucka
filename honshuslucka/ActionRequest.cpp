#include <string>
#include "ActionRequest.h"

ActionRequest::ActionRequest(std::string name) {

	_name = name;

	_initialized = false;
};

void ActionRequest::initialize() {
	if (!_initialized) {
		// https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/

		_initialized = true;
	}
}

std::string ActionRequest::getName() {
	return _name;
};

bool ActionRequest::getAcknowledged() {
	return _acknowledged;
};

void ActionRequest::setAcknowledged(bool value) {
	_acknowledged = value;
};

ActuatorAction ActionRequest::getAction() {
	return _action;
};

void ActionRequest::setAction(ActuatorAction value) {
	_action = value;
};

int ActionRequest::getPriority() {
	return _priority;
};
void ActionRequest::setPriority(int value) {
	_priority = value;
};