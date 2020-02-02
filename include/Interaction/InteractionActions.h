#pragma once
#include "Action.h"
#include "Expression.h"

class AssignAction : public Action {
private:
	std::shared_ptr<Expression> _expression;
	std::vector<std::string> _postfix;
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > _arguments;
public:
	AssignAction();
	bool addArgument(std::shared_ptr<Component> argument, std::string name);
	bool addArgument(std::string name);
	bool setCondition(std::string condition);

	bool initializeAction();
	bool doAction();
};