#include "Component.h"

Component::Component() {
	_componentName = "Component";
}

std::string Component::getName() {
	return _componentName;
}

OperationComponent::OperationComponent() {
	_componentName = "Component";
}

std::tuple<float*, bool> OperationComponent::getMemberFloat(std::string name, int index) {
	float* value = nullptr;
	bool correctness = false;
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end()) {
		value = _classVariablesFloat[name];
		correctness = true;
	}
	float* result;
	if (index < 0)
		result = value;
	else
		result = &value[index];

	return { result, correctness };
}

std::tuple<std::string*, bool> OperationComponent::getMemberString(std::string name, int index) {
	std::string* value = nullptr;
	bool correctness = false;
	if (_classVariablesString.find(name) != _classVariablesString.end()) {
		value = _classVariablesString[name];
		correctness = true;
	}
	std::string* result;
	if (index < 0)
		result = value;
	else
		result = &value[index];

	return { result, correctness };
}

VariableType OperationComponent::getVariableType(std::string name) {
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end())
		return VariableType::varFloat;

	if (_classVariablesString.find(name) != _classVariablesString.end())
		return VariableType::varString;

	return VariableType::varUnknown;
}

bool OperationComponent::setMember(std::string name, float value, int index) {
	if (index < 0) {
		*_classVariablesFloat[name] = value;
	}
	else {
		_classVariablesFloat[name][index] = value;
	}
	return false;
}

bool OperationComponent::setMember(std::string name, std::string value, int index) {
	if (index < 0) {
		*_classVariablesString[name] = value;
	}
	else {
		_classVariablesString[name][index] = value;
	}
	return false;
}