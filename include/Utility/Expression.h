#pragma once
#include "Component.h"
#include "Entity.h"
#include "View.h"

class ExpressionNode {
private:
	std::string _value;
	std::vector<std::shared_ptr<ExpressionNode> > _nodes;
public:
	ExpressionNode(std::string value);
	bool setNode(std::shared_ptr<ExpressionNode> node);
	std::string getValue();
	std::vector<std::shared_ptr<ExpressionNode> > getNodes();
};

class Expression {
	std::string _condition;
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string, int> > _supportedOperations;
public:
	Expression();
	bool addSupportedOperation(std::string name, std::tuple<int, std::string, int> operation);
	std::map<std::string, std::tuple<int, std::string, int> > getSupportedOperations();

	bool setCondition(std::string condition);
	std::string getCondition();

	bool prepareExpression(std::shared_ptr<ExpressionNode>& postfix);
	std::tuple<std::string, int> arithmeticOperationFloat(float operand[2], std::string operation);
	std::tuple<std::string, int> arithmeticOperationString(std::string operand[2], std::string operation);
	std::tuple<std::string, int> oneArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item, std::string operation);
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int, int> twoArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item1,
																								 std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item2,
																								 std::string operation);
	std::tuple<std::shared_ptr<OperationComponent>, std::string, int, int> threeArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item1,
																								  std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item2,
																								  std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item3,
																								  std::string operation);
	std::tuple<std::string, int> multipleArgumentOperation(std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > batch, std::string operation);
};

