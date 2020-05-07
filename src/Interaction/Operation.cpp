#include "Operation.h"
#include "Entity.h"
#include <algorithm>

Operation::Operation(std::string name) {
	_name = name;
}

std::string Operation::getName() {
	return _name;
}

Operation::~Operation() {

}

std::vector<std::shared_ptr<Action> > Operation::getActions() {
	return _actions;
}

void Operation::registerAction(std::shared_ptr<Action> listener) {
	_actions.push_back(listener);
}

void Operation::unregisterAction(std::shared_ptr<Action> listener) {
	if (listener && _actions.size() > 0)
		_actions.erase(std::remove(_actions.begin(), _actions.end(), listener), _actions.end());
}