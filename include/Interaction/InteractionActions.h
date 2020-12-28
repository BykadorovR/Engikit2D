#pragma once
#include "Action.h"
#include "Expression.h"

class AssignAction : public Action {
private:
	std::shared_ptr<Expression> _expression;
public:
	AssignAction(std::string name = "AssignAction");
	std::string addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name);
	
	bool initializeAction(std::string condition);
	bool doAction();
};