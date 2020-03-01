#pragma once
#include <string>
#include <map>
#include <vector>

enum VariableType {
	varUnknown = 0,
	varFloat = 1,
	varString = 2
};

class Component {
protected:
	std::map<std::string, float*> _classVariablesFloat;
	std::map<std::string, std::string*> _classVariablesString;
	std::string _componentName;
public:
	Component();
	
	virtual bool setMember(std::string name, float value);
	virtual bool setMember(std::string name, std::string value);

	VariableType getVariableType(std::string name);
	std::string getName();

	std::tuple<float*, bool> getMemberFloat(std::string name);
	std::tuple<std::string*, bool> getMemberString(std::string name);

	virtual ~Component() = default;
};
