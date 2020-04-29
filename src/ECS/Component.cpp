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

std::tuple<float*, bool> OperationComponent::getMemberFloat(std::string name) {
	std::tuple<float*, int> value = {nullptr, -1};
	bool correctness = false;
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end()) {
		value = _classVariablesFloat[name];
		correctness = true;
	}
	float* result;
	if (std::get<1>(value) < 0)
		result = std::get<0>(value);
	else
		result = &std::get<0>(value)[std::get<1>(value)];

	return { result, correctness };
}

std::tuple<std::string*, bool> OperationComponent::getMemberString(std::string name) {
	std::tuple<std::string*, int> value = { nullptr, -1 };
	bool correctness = false;
	if (_classVariablesString.find(name) != _classVariablesString.end()) {
		value = _classVariablesString[name];
		correctness = true;
	}
	std::string* result;
	if (std::get<1>(value) < 0)
		result = std::get<0>(value);
	else
		result = &std::get<0>(value)[std::get<1>(value)];

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
		*std::get<0>(_classVariablesFloat[name]) = value;
	}
	else {
		std::get<0>(_classVariablesFloat[name])[std::get<1>(_classVariablesFloat[name])] = value;
	}
	return false;
}

bool OperationComponent::setMember(std::string name, std::string value, int index) {
	if (index < 0) {
		*std::get<0>(_classVariablesString[name]) = value;
	}
	else {
		std::get<0>(_classVariablesString[name])[std::get<1>(_classVariablesString[name])] = value;
	}
	return false;
}