#pragma once
#include "Component.h"
#include "Entity.h"

class Expression {
	std::string _condition;
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
public:
	Expression(std::map<std::string, std::tuple<int, std::string> > supportedOperations);
	bool setCondition(std::string condition);
	std::string getCondition();

	bool prepareExpression(std::vector<std::string>& postfix);
	bool entityOperation(std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> >& intermediate, std::shared_ptr<Entity> entity, std::string operation);
	bool arithmeticOperationFloat(std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> >& intermediate, float operand[2], std::string operation);
	bool arithmeticOperationString(std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> >& intermediate, std::string operand[2], std::string operation);
};

