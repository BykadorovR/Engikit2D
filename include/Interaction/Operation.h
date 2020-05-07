#pragma once
#include <string>
#include <vector>
#include "Action.h"
#include "Expression.h"

class Operation {
protected:
	std::string _name;
	std::vector<std::shared_ptr<Action> > _actions;
public:
	Operation(std::string name = "Operation");
	virtual bool checkOperation() {
		return false;
	};
	std::vector<std::shared_ptr<Action> > getActions();
	void registerAction(std::shared_ptr<Action> listener);
	void unregisterAction(std::shared_ptr<Action> listener);
	std::string getName();
	virtual ~Operation() = 0;
};