#include "CustomComponents.h"

CustomStringArrayComponent::CustomStringArrayComponent() {
	_componentName = "CustomStringArrayComponent";
}

bool CustomStringArrayComponent::addCustomValue(std::string value, std::string name) {
	//no vector with such name was initialized yet
	if (_strings.find(name) == _strings.end()) {
		//if no records with such name exist so we can add it
		if (_classVariablesVectorString.find(name) == _classVariablesVectorString.end()) {
			_classVariablesVectorString[name] = &_strings[name];
		}
		else {
			//can't add
			return true;
		}
	}
	_strings[name].push_back(value);

	return false;
}

CustomFloatComponent::CustomFloatComponent() {
	_componentName = "CustomFloatComponent";
}

//TODO: make adequate return statuses
bool CustomFloatComponent::addCustomValue(float value, std::string name) {
	if (_floats.find(name) == _floats.end()) {
		if (_classVariablesFloat.find(name) == _classVariablesFloat.end()) {
			_classVariablesFloat[name] = &value;
		}
		else {
			return true;
		}
	}
	_floats[name] = value;
	return false;
}
