#pragma once
#include <string>
#include <map>
#include <vector>

#include "Operation.h"
#include "Action.h"

class Component {
protected:
	std::map<std::string, float*> _classVariables;
	std::string _componentName;
public:
	Component();
	std::tuple<float, bool> getMember(std::string name);
	std::string getName();
	virtual ~Component() = default;
};
