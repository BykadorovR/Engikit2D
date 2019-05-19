#pragma once
#include "General.h"
#include <functional>
/*
Needed to store user created components in container
*/
class Component {
public:
	int _componentID;
	virtual ~Component() = default;
};