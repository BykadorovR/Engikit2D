#pragma once
#include <string>
#include <map>
#include <vector>

class Component {
protected:
	std::map<std::string, float*> _classVariables;
	std::string _componentName;
public:
	Component();
	std::tuple<float*, bool> getMember(std::string name);
	virtual bool setMember(std::string name, float value);
	std::string getName();
	virtual ~Component() = default;
};
