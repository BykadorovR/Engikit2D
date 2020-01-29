#pragma once
#include "Expression.h"
#include "Operation.h"
#include <algorithm>

class SimpleOperation : public Operation {
private:
	std::shared_ptr<Expression> _expression;
public:
	void setExpression(std::shared_ptr<Expression> expression);
	bool checkOperation();
	void registerAction(std::shared_ptr<Action> listener);
	void unregisterAction(std::shared_ptr<Action> listener);
};