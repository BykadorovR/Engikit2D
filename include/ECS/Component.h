#pragma once
#include <string>
#include <map>
#include <vector>

#include "Operation.h"
#include "Action.h"

class Component {
public:
	Component();
	std::string _componentName;
	virtual ~Component() = default;
};