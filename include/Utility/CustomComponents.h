#pragma once
#include "Component.h"

class CustomStringArrayComponent : public OperationComponent {
public:
	CustomStringArrayComponent();
	bool addCustomValue(std::string value, std::string name);
private:
	//should be extended to map or vector of vectors
	std::map<std::string, std::vector<std::string> > _strings;
};

class CustomFloatComponent : public OperationComponent {
public:
	CustomFloatComponent();
	bool addCustomValue(float value, std::string name);
private:
	std::map<std::string, float > _floats;
};