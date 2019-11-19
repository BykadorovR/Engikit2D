#pragma once
#include <string>
#include <map>
#include <vector>

#include "Operation.h"
#include "Action.h"

class Component {
protected:
	std::map<Operation, std::vector<Action> > _interaction;
public:
	Component();
	std::string _componentName;
	virtual ~Component() = default;
};