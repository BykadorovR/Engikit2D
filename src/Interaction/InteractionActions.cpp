#include "InteractionActions.h"
#include <assert.h>
#include <regex>
#include "Common.h"

AssignAction::AssignAction(std::string name) {
	_name = name;
	_expression = std::make_shared<Expression>();
	_expression->addSupportedOperation("SET",  { 1, "left", 2 });
}

std::string AssignAction::addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name) {
	_expression->addArgument({ entity, component, name });
	return std::to_string(_expression->getArguments().size() - 1);
}


bool AssignAction::initializeAction(std::string condition) {
	return _expression->prepareExpression(condition);
}

bool AssignAction::doAction() {
	return stof(std::get<2>(_expression->evaluateExpression(_expression->getPostfix()).back()));
}