#include "UtilityComponents.h"

GroupComponent::GroupComponent() {
	_componentName = "GroupComponent";
	_classVariablesString =
	{
		{"groupName", &_groupName}
	};
}

bool GroupComponent::initialize(std::string groupName) {
	_groupName = groupName;
	return false;
}

bool GroupComponent::setGroupName(std::string name) {
	_groupName = name;
	return false;
}

std::string GroupComponent::getGroupName() {
	return _groupName;
}