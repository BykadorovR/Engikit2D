#pragma once
#include <string>
#include <map>
#include <vector>

class Component {
protected:
	std::string _componentName;
public:
	Component();
	std::string getName();
	virtual ~Component() = default;
};

enum VariableType {
	varUnknown = 0,
	varFloat = 1,
	varString = 2
};

class OperationComponent : public Component {
protected:
	std::map<std::string, std::tuple<float*, int> > _classVariablesFloat;
	std::map<std::string, std::tuple<std::string*, int> > _classVariablesString;
	
public:
	OperationComponent();
	
	//if prototype is changed (new arguments added/removed), need to do the same in derived classes 
	virtual bool setMember(std::string name, float value, int index = -1);
	virtual bool setMember(std::string name, std::string value, int index = -1);

	VariableType getVariableType(std::string name);

	std::tuple<float*, bool> getMemberFloat(std::string name);
	std::tuple<std::string*, bool> getMemberString(std::string name);
};
