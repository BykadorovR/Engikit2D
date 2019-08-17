#pragma once
#include "General.h"
#include <functional>
#include "GUISave.h"
/*
Needed to store user created components in container
*/
class Component {
public:
	std::string _componentName = "Component";
	virtual ~Component() = default;
};