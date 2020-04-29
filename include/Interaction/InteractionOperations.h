#pragma once
#include "Expression.h"
#include "Operation.h"
#include <algorithm>
#include "Entity.h"

class ExpressionOperation : public Operation {
private:
	std::shared_ptr<Expression> _expression;

	std::vector<std::string> _postfix;
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string> > _arguments;
	std::vector<std::shared_ptr<Entity> > _entities;
public:
	ExpressionOperation();
	bool addArgument(std::shared_ptr<OperationComponent> argument, std::string name);
	bool addArgument(std::shared_ptr<Entity> entity);
	
	bool initializeOperation(std::string condition);
	bool checkOperation();
};