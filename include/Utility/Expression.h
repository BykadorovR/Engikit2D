#pragma once
#include "Component.h"

class Expression {
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > _arguments;
	std::string _condition;
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
public:
	Expression();
	bool addArgument(std::shared_ptr<Component> argument, std::string name);
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > getArguments();
	bool setCondition(std::string condition);
	std::string getCondition();
	bool calculateExpression(float* evaluatedResult);
};