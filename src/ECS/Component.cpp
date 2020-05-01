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
	float* value = nullptr;
	bool correctness = false;
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end()) {
		value = _classVariablesFloat[name];
		correctness = true;
	}
	
	return { value, correctness };
}

std::tuple<std::string*, bool> OperationComponent::getMemberString(std::string name) {
	std::string* value = nullptr;
	bool correctness = false;
	if (_classVariablesString.find(name) != _classVariablesString.end()) {
		value = _classVariablesString[name];
		correctness = true;
	}
	
	return { value, correctness };
}

std::tuple<std::vector<float>*, bool> OperationComponent::getMemberVectorFloat(std::string name) {
	if (_classVariablesVectorFloat.find(name) != _classVariablesVectorFloat.end()) {
		return {_classVariablesVectorFloat[name], true};
	}
	return {nullptr, false};
}

std::tuple<std::vector<std::string>*, bool> OperationComponent::getMemberVectorString(std::string name) {
	if (_classVariablesVectorString.find(name) != _classVariablesVectorString.end()) {
		return {_classVariablesVectorString[name], true};
	}
	return {nullptr, false};
}

VariableType OperationComponent::getVariableType(std::string name) {
	if (_classVariablesFloat.find(name) != _classVariablesFloat.end())
		return VariableType::varFloat;

	if (_classVariablesVectorFloat.find(name) != _classVariablesVectorFloat.end())
		return VariableType::varFloatVector;

	if (_classVariablesString.find(name) != _classVariablesString.end())
		return VariableType::varString;

	if (_classVariablesVectorString.find(name) != _classVariablesVectorString.end())
		return VariableType::varStringVector;

	return VariableType::varUnknown;
}

bool OperationComponent::setMember(std::string name, float value, int index) {
	if (index < 0) {
		*_classVariablesFloat[name] = value;
	}
	else {
		_classVariablesVectorFloat[name]->at(index) = value;
	}
	return false;
}

bool OperationComponent::setMember(std::string name, std::string value, int index) {
	if (index < 0) {
		*_classVariablesString[name] = value;
	}
	else {
		_classVariablesVectorString[name]->at(index) = value;
	}
	return false;
}