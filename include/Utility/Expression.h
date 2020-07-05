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
	//name priority associativity
	std::map<std::string, std::tuple<int, std::string, int> > _supportedOperations;
	std::shared_ptr<ExpressionNode> _postfix;
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string> > _arguments;
public:
	Expression();
	bool addSupportedOperation(std::string name, std::tuple<int, std::string, int> operation);
	std::map<std::string, std::tuple<int, std::string, int> > getSupportedOperations();

	bool addArgument(std::tuple<std::shared_ptr<Entity>, std::string, std::string> argument);
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string> > getArguments();

	std::shared_ptr<ExpressionNode> getPostfix();

	bool prepareExpression(std::string condition);
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > > evaluateExpression(std::shared_ptr<ExpressionNode> node);

	std::tuple<std::string, int> arithmeticOperationFloat(float operand[2], std::string operation);
	std::tuple<std::string, int> arithmeticOperationString(std::string operand[2], std::string operation);
	std::tuple<std::string, int> oneArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item, std::string operation);
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int>, int> twoArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item1,
																								 std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item2,
																								 std::string operation);
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int>, int> threeArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item1,
																								   std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item2,
																								   std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > item3,
																								   std::string operation);
	std::tuple<std::string, int> multipleArgumentOperation(std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, std::vector<int> > > batch, std::string operation);
};

