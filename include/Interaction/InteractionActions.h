#pragma once
#include "Action.h"
#include "Expression.h"
#include "Scene.h"
#include "Decorator.h"


class AssignAction : public Action {
private:
	std::shared_ptr<Expression> _expression;
	std::vector<std::string> _postfix;
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string> > _arguments;
public:
	AssignAction(std::string actionName = "AssignAction");
	bool addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name);
	
	bool initializeAction(std::string condition);
	bool doAction();
};