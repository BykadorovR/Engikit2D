#pragma once
#include <string>
#include <vector>
#include "Action.h"

class Operation {
protected:
	std::vector<std::shared_ptr<Action> > _actions;
public:
	virtual bool checkOperation() = 0;
	virtual void registerAction(std::shared_ptr<Action> listener) = 0;
	virtual void unregisterAction(std::shared_ptr<Action> listener) = 0;
	virtual ~Operation() = 0;
};