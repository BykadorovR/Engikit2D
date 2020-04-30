#pragma once
#include "Action.h"
#include "Expression.h"
#include "Scene.h"
#include "Decorator.h"


class AssignAction : public Action {
private:
	std::shared_ptr<Expression> _expression;
	std::vector<std::string> _postfix;
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > _arguments;
public:
	AssignAction();
	bool addArgument(std::shared_ptr<OperationComponent> argument, std::string name, int index = -1);
	
	bool initializeAction(std::string condition);
	bool doAction();
};