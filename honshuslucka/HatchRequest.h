#include <Arduino.h>
#include "HatchRequestAction.h"

#ifndef HATCH_REQUEST_H
#define HATCH_REQUEST_H

class HatchRequest {

private:
	String _name;

	bool _acknowledged;

	HatchRequestAction _action;

	bool _initialized;

public:
	HatchRequest(String name);

	void initialize();

	bool getInitialized();

	bool getAcknowledged();
	void setAcknowledged(bool value);

	HatchRequestAction getAction();
	void setAction(HatchRequestAction value);

	String getName();
};

#endif