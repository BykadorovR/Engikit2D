#include "Component.h"

Component::Component() {
	_componentName = "Component";
}

std::tuple<float, bool> Component::getMember(std::string name) {
	float value = 0;
	bool correctness = false;
	if (_classVariables.find(name) != _classVariables.end()) {
		value = *_classVariables[name];
		correctness = true;
	}
	return { value, correctness };
}


std::string Component::getName() {
	return _componentName;
}