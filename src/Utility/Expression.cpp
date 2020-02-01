#include "Expression.h"

bool Expression::addArgument(std::shared_ptr<Component> argument, std::string name) {
	_arguments.push_back({ argument, name });
	return false;
}

std::vector<std::tuple<std::shared_ptr<Component>, std::string> > Expression::getArguments() {
	return _arguments;
}

bool Expression::setCondition(std::string condition) {
	_condition = condition;
	return false;
}

std::string Expression::getCondition() {
	return _condition;
}