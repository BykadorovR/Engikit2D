#include "InteractionOperations.h"
#include <regex>
#include <assert.h>
#include "Common.h"
#include <sstream>

ExpressionOperation::ExpressionOperation(std::string name) {
	_name = name;
	_expression = std::make_shared<Expression>();
}

std::string ExpressionOperation::addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name) {
	_expression->addArgument({ entity, component, name });
	return std::to_string(_expression->getArguments().size() - 1);
}

std::string ExpressionOperation::addArgument(std::vector<std::shared_ptr<Entity> > entities) {
	std::string listIndexes = "";
	for (auto item = entities.begin(); item != entities.end(); item++) {
		listIndexes += addArgument(*item, "", "");
		if (std::next(item) != entities.end())
			listIndexes += ",";
	}
	return listIndexes;
}

bool ExpressionOperation::initializeOperation(std::string condition) {
	return _expression->prepareExpression(condition);
}

bool ExpressionOperation::checkOperation() {
	auto result = _expression->evaluateExpression(_expression->getPostfix()).back();
	return stof(std::get<2>(result));
}