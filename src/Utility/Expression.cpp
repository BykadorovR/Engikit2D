#include "Expression.h"
#include <assert.h>
#include "UserInputComponents.h"
#include "GraphicComponents.h"

Expression::Expression(std::map<std::string, std::tuple<int, std::string> > supportedOperations) {
	_supportedOperations = supportedOperations;
}

bool Expression::setCondition(std::string condition) {
	_condition = condition;
	return false;
}

std::string Expression::getCondition() {
	return _condition;
}

//operation[0] - operation from back, so it's actually second operation then operation[1] - first operation
std::tuple<std::string, int> Expression::arithmeticOperationFloat(float operand[2], std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "+")
		result = { std::to_string(operand[1] + operand[0]), 1 };
	else if (operation == "-")
		result = { std::to_string(operand[1] - operand[0]), 1 };
	else if (operation == "*")
		result = { std::to_string(operand[1] * operand[0]), 1 };
	else if (operation == "/") {
		assert(operand[0] != 0, "Division by zero");
		result = { std::to_string(operand[1] / operand[0]), 1 };
	}
	else if (operation == "=")
		result = { std::to_string(operand[1] == operand[0]), 1 };
	else if (operation == ">")
		result = { std::to_string(operand[1] > operand[0]), 1 };
	else if (operation == "<")
		result = { std::to_string(operand[1] < operand[0]), 1 };
	else if (operation == "AND")
		result = { std::to_string(operand[1] && operand[0]), 1 };
	else if (operation == "OR")
		result = { std::to_string(operand[1] || operand[0]), 1 };
	else
		result = { "", 0 };

	return result;
}

//operation[0] - operation from back, so it's actually second operation then operation[1] - first operation
std::tuple<std::string, int> Expression::arithmeticOperationString(std::string operand[2], std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "+")
		result = { (operand[1] + operand[0]), 1 };
	else if (operation == "=")
		result = { std::to_string(operand[1] == operand[0]), 1 };
	else if (operation == ">")
		result = { std::to_string(operand[1] > operand[0]), 1 };
	else if (operation == "<")
		result = { std::to_string(operand[1] < operand[0]), 1 };
	else
		result = { "", 0 };

	return result;
}

std::tuple<std::string, int> Expression::oneArgumentOperation(std::shared_ptr<Entity> entity, std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "CLICK") {
		auto mouseComponent = entity->getComponent<MouseComponent>();
		auto objectComponent = entity->getComponent<ObjectComponent>();
		if (mouseComponent && objectComponent) {
			float x = std::get<0>(objectComponent->getPosition());
			float y = std::get<1>(objectComponent->getPosition());
			float width = std::get<0>(objectComponent->getSize());
			float height = std::get<1>(objectComponent->getSize());
			auto clickX = mouseComponent->getMemberFloat("leftClickX");
			auto clickY = mouseComponent->getMemberFloat("leftClickY");
			if (!std::get<1>(clickX) || !std::get<1>(clickY))
				result = { std::to_string(false), 1 };

			if (*std::get<0>(clickX) > x && *std::get<0>(clickX) < x + width && *std::get<0>(clickY) > y && *std::get<0>(clickY) < y + height) {
				result = { std::to_string(true), 1 };
			}
			else {
				result = { std::to_string(false), 1 };
			}
		}
		else {
			//set false as result
			result = { std::to_string(false), 1 };
		}
	}
	else
		result = { std::to_string(false), 0 };

	return result;
}

std::tuple<std::string, int> Expression::oneArgumentOperation(std::shared_ptr<View> view, std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "CLICK") {
		bool anyClick = false;
		for (auto view : view->getViews()) {
			auto entity = view->getEntity();
			if (!std::get<0>(entity->getComponent<ObjectComponent>()->getMemberFloat("visible"))) {
				continue;
			}
			if (std::get<1>(oneArgumentOperation(entity, operation)) && atoi(std::get<0>(oneArgumentOperation(entity, operation)).c_str())) {
				anyClick = true;
				break;
			}
		}
		if (anyClick)
			result = { std::to_string(true), 1 };
		else
			result = { std::to_string(false), 1 };
	} else
		result = { std::to_string(false), 0 };

	return result;
}

bool Expression::prepareExpression(std::vector<std::string>& postfix) {
	//first we need to convert condition to postfix form
	//Shunting-yard algorithm
	std::vector<std::string> operatorStack;
	std::vector<std::string> splitedInput;
	//first of all we should split our operation by separator
	int wordIndex = 0;
	splitedInput.push_back("");
	for (auto c = _condition.begin(); c < _condition.end(); c++) {
		if (*c == *(" ")) {
			wordIndex++;
			splitedInput.push_back("");
			continue;
		}
		splitedInput[wordIndex] += *c;
	}

	//let's form postfix notation
	for (auto word = splitedInput.begin(); word < splitedInput.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			//operator at the top of the operator stack is not a left parenthesis
			while (operatorStack.size() > 0 && (operatorStack.back() != "(") &&
				//operator at the top of the operator stack with greater precedence
				((std::get<0>(_supportedOperations[operatorStack.back()]) > std::get<0>(_supportedOperations[*word])) ||
					//operator at the top of the operator stack has equal precedence and the token is left associative
				(std::get<0>(_supportedOperations[operatorStack.back()]) == std::get<0>(_supportedOperations[*word]) && std::get<1>(_supportedOperations[*word]) == "left")))
			{
				//pop operators from the operator stack onto the output queue.
				postfix.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			operatorStack.push_back(*word);
		}
		else if (*word == "(") {
			operatorStack.push_back(*word);
		}
		else if (*word == ")") {
			while (operatorStack.back() != "(") {
				postfix.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			//if the stack runs out without finding a left paren, then there are mismatched parentheses
			if (operatorStack.size() == 0)
				return true;
			//discard
			if (operatorStack.back() == "(") {
				operatorStack.pop_back();
			}
		}
		//word (token) is number or variable
		else {
			postfix.push_back(*word);
		}
	}

	//pop remainings operators from the operator stack onto the output queue.
	while (operatorStack.size() > 0) {
		postfix.push_back(operatorStack.back());
		operatorStack.pop_back();
	}
}