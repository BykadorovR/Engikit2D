#pragma once
#include <string>
#include <map>
#include <vector>

#include "Operation.h"
#include "Action.h"

class Component {
private:
	std::string _componentName;
public:
	Component();
	//TODO: std::tuple<float, bool>
	virtual float getMember(std::string name) = 0;
	virtual ~Component() = default;
};
