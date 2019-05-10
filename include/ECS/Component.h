#pragma once
#include "General.h"
#include "Entity.h"
#include <functional>
/*
Needed to store user created components in container
*/
class Component {
public:
	virtual ~Component() = default;
};