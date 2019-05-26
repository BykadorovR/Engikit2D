#pragma once
#include "General.h"
#include <functional>
#include "GUISave.h"
/*
Needed to store user created components in container
*/
class Component {
public:
	int _componentID = 0;
	virtual ~Component() = default;
};