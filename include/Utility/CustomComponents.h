#pragma once
#include "Component.h"

class CustomStringArrayComponent : public OperationComponent {
public:
	CustomStringArrayComponent();
	bool initializeEmpty(std::string name);
	bool addCustomValue(std::string value, std::string name);
	bool clear(std::string name);
private:
	//should be extended to map or vector of vectors
	std::map<std::string, std::vector<std::string> > _strings;
};

class CustomFloatArrayComponent : public OperationComponent {
public:
	CustomFloatArrayComponent();
	bool initializeEmpty(std::string name);
	bool addCustomValue(float value, std::string name);
	bool clear(std::string name);
private:
	//should be extended to map or vector of vectors
	std::map<std::string, std::vector<float> > _floats;
};

class CustomFloatComponent : public OperationComponent {
public:
	CustomFloatComponent();
	bool initializeEmpty(std::string name);
	bool addCustomValue(float value, std::string name);
private:
	std::map<std::string, float > _floats;
};