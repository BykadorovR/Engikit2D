#pragma once
#include "Component.h"

class Expression {
	std::string _condition;
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
public:
	Expression(std::map<std::string, std::tuple<int, std::string> > supportedOperations);
	bool setCondition(std::string condition);
	std::string getCondition();

	bool prepareExpression(std::vector<std::string>& postfix);
};

