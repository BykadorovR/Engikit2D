#include "Component.h"

Component::Component() {
	_componentName = "Component";
}

std::tuple<float*, bool> Component::getMemberFloat(std::string name) {
	float* value = 0;
	bool correctness = false;
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end()) {
		value = _classVariablesFloat[name];
		correctness = true;
	}
	return { value, correctness };
}

std::tuple<std::string*, bool> Component::getMemberString(std::string name) {
	std::string* value = 0;
	bool correctness = false;
	if (_classVariablesString.find(name) != _classVariablesString.end()) {
		value = _classVariablesString[name];
		correctness = true;
	}
	return { value, correctness };
}

VariableType Component::getVariableType(std::string name) {
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end())
		return VariableType::varFloat;

	if (_classVariablesString.find(name) != _classVariablesString.end())
		return VariableType::varString;

	return VariableType::varUnknown;
}

bool Component::setMember(std::string name, float value) {
	*_classVariablesFloat[name] = value;
	return false;
}

bool Component::setMember(std::string name, std::string value) {
	*_classVariablesString[name] = value;
	return false;
}

std::string Component::getName() {
	return _componentName;
}