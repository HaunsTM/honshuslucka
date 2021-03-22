#include <string>
#include "ActuatorAction.h"

#ifndef ACTION_REQUEST_H
#define ACTION_REQUEST_H

class ActionRequest {

private:
	std::string _name;

	bool _acknowledged;

	ActuatorAction _action;

	ActuatorAction _previousAction;

	int _priority;
	bool _initialized;

public:
	ActionRequest(std::string name);

	void initialize();

	bool getAcknowledged();
	void setAcknowledged(bool value);

	ActuatorAction getAction();
	void setAction(ActuatorAction value);

	int getPriority();
	void setPriority(int value);

	std::string getName();
};

#endif