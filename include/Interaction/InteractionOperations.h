#pragma once
#include "Expression.h"
#include "Operation.h"
#include <algorithm>
#include "Entity.h"
#include "View.h"

//TODO: merge common part with arguments with action
class ExpressionOperation : public Operation {
private:
	std::shared_ptr<Expression> _expression;

	std::vector<std::string> _postfix;
	std::map<int, std::tuple<std::shared_ptr<Entity>, std::string, std::string> > _arguments;
	std::map<int, std::shared_ptr<View> > _views;
public:
	ExpressionOperation(std::string name = "ExpressionOperation");
	std::string addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name);
	std::string addArgument(std::vector<std::shared_ptr<Entity> > entities);

	bool initializeOperation(std::string condition);
	bool checkOperation();
};