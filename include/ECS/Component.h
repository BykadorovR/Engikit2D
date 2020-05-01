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
	varString = 2,
	varFloatVector = 3,
	varStringVector = 4
};

class OperationComponent : public Component {
protected:
	std::map<std::string, float*> _classVariablesFloat;
	std::map<std::string, std::string* > _classVariablesString;
	std::map<std::string, std::vector<float>* > _classVariablesVectorFloat;
	std::map<std::string, std::vector<std::string>* > _classVariablesVectorString;
	
public:
	OperationComponent();
	
	//if prototype is changed (new arguments added/removed), need to do the same in derived classes 
	virtual bool setMember(std::string name, float value, int index = -1);
	virtual bool setMember(std::string name, std::string value, int index = -1);

	VariableType getVariableType(std::string name);

	std::tuple<float*, bool> getMemberFloat(std::string name, int index = -1);
	std::tuple<std::string*, bool> getMemberString(std::string name, int index = -1);
	std::tuple<std::vector<float>*, bool> getMemberVectorFloat(std::string name);
	std::tuple<std::vector<std::string>*, bool> getMemberVectorString(std::string name);
};
