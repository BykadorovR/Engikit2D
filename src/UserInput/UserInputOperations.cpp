#include "UserInputOperations.h"

void SimpleOperation::setExpression(std::shared_ptr<Expression> expression) {
	_expression = expression;
}

bool SimpleOperation::checkOperation() {
	//need to parse expression
	//_expression->getOperand()->getMember(name)
	return false;
}

void SimpleOperation::registerAction(std::shared_ptr<Action> listener) {
	_actions.push_back(listener);
}

void SimpleOperation::unregisterAction(std::shared_ptr<Action> listener) {
	if (listener && _actions.size() > 0)
		_actions.erase(std::remove(_actions.begin(), _actions.end(), listener), _actions.end());
}