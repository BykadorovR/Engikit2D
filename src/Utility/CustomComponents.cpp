#include "CustomComponents.h"

CustomArrayComponent::CustomArrayComponent() {
	_componentName = "CustomArrayComponent";
}

bool CustomArrayComponent::addCustomValue(std::string value, std::string name) {
	//no vector with such name was initialized yet
	if (_strings.find(name) == _strings.end()) {
		//if no records with such name exist so we can add it
		if (_classVariablesString.find(name) == _classVariablesString.end()) {
			_classVariablesString[name] = _strings[name].data();
		}
		else {
			//can't add
			return true;
		}
	}
	_strings[name].push_back(value);

	auto test =_classVariablesString[name][0];
	return false;
}