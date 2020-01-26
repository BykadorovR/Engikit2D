#pragma once
#include <string>
#include <vector>
#include "Action.h"

class Operation {
protected:
	std::vector<Action* > _actions;
public:
	virtual void registerAction(Action* listener) = 0;
	virtual void unregisterAction(Action* listener) = 0;
	virtual ~Operation() = 0;
};