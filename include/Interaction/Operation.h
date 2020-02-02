#pragma once
#include <string>
#include <vector>
#include "Action.h"
#include "Expression.h"

class Operation {
protected:
	std::vector<std::shared_ptr<Action> > _actions;
public:
	virtual bool checkOperation() = 0;
	std::vector<std::shared_ptr<Action> > getActions();
	void registerAction(std::shared_ptr<Action> listener);
	void unregisterAction(std::shared_ptr<Action> listener);
	virtual ~Operation() = 0;
};