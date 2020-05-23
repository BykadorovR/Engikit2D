#pragma once
#include "Component.h"
#include "Entity.h"
#include "View.h"

class Expression {
	std::string _condition;
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
public:
	Expression();
	bool addSupportedOperation(std::string name, std::tuple<int, std::string> operation);
	std::map<std::string, std::tuple<int, std::string> > getSupportedOperations();

	bool setCondition(std::string condition);
	std::string getCondition();

	bool prepareExpression(std::vector<std::string>& postfix);
	std::tuple<std::string, int> arithmeticOperationFloat(float operand[2], std::string operation);
	std::tuple<std::string, int> arithmeticOperationString(std::string operand[2], std::string operation);
	std::tuple<std::string, int> oneArgumentOperation(std::shared_ptr<Entity> entity, std::string operation);
	std::tuple<std::string, int> oneArgumentOperation(std::vector<std::shared_ptr<Entity> > batch, std::string operation);
	std::tuple<std::string, int> oneArgumentOperation(std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item, std::string operation);
	std::tuple<std::shared_ptr<OperationComponent>, std::string, int, int> twoArgumentOperation(std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item1, 
																								std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item2,
																								std::string operation);
	std::tuple<std::shared_ptr<OperationComponent>, std::string, int, int> threeArgumentOperation(std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item1, 
																								  std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item2,
																								  std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item3, 
																								  std::string operation);
};

