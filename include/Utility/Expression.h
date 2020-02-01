#pragma once
#include "Component.h"

class Expression {
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > _arguments;
	std::string _condition;
public:
	bool addArgument(std::shared_ptr<Component> argument, std::string name);
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > getArguments();
	bool setCondition(std::string expression);
	std::string getCondition();
};