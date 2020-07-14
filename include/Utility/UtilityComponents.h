#pragma once
#include "Component.h"

class GroupComponent : public OperationComponent {
private:
	std::string _groupName;
public:
	GroupComponent();
	bool initialize(std::string groupName);

	bool setGroupName(std::string name);
	std::string getGroupName();
};
