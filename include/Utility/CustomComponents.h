#pragma once
#include "Component.h"

class CustomArrayComponent : public OperationComponent {
public:
	CustomArrayComponent();
	bool addCustomValue(std::string value, std::string name);
private:
	//should be extended to map or vector of vectors
	std::map<std::string, std::vector<std::string> > _strings;
};

