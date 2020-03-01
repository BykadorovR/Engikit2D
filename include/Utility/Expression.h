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
	bool arithmeticOperationFloat(std::vector<std::tuple<std::shared_ptr<Component>, std::string> >& intermediate, float operand[2], std::string operation);
	bool arithmeticOperationString(std::vector<std::tuple<std::shared_ptr<Component>, std::string> >& intermediate, std::string operand[2], std::string operation);
};

