#include <string>
#include "HatchRequestAction.h"

#ifndef HATCH_REQUEST_H
#define HATCH_REQUEST_H

class HatchRequest {

private:
	std::string _name;

	bool _acknowledged;

	HatchRequestAction _action;

	bool _initialized;

public:
	HatchRequest(std::string name);

	void initialize();

	bool getAcknowledged();
	void setAcknowledged(bool value);

	HatchRequestAction getAction();
	void setAction(HatchRequestAction value);

	std::string getName();
};

#endif