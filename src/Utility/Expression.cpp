#include "Expression.h"

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
bool Expression::arithmeticOperationFloat(std::vector<std::tuple<std::shared_ptr<Component>, std::string> >& intermediate, float operand[2], std::string operation) {
	if (operation == "+")
		intermediate.push_back({ nullptr, std::to_string(operand[1] + operand[0]) });
	else if (operation == "-")
		intermediate.push_back({ nullptr, std::to_string(operand[1] - operand[0]) });
	else if (operation == "*")
		intermediate.push_back({ nullptr, std::to_string(operand[1] * operand[0]) });
	else if (operation == "/")
		intermediate.push_back({ nullptr, std::to_string(operand[1] / operand[0]) });
	else if (operation == "=")
		intermediate.push_back({ nullptr, std::to_string(operand[1] == operand[0]) });
	else if (operation == ">")
		intermediate.push_back({ nullptr, std::to_string(operand[1] > operand[0]) });
	else if (operation == "<")
		intermediate.push_back({ nullptr, std::to_string(operand[1] < operand[0]) });
	else if (operation == "AND")
		intermediate.push_back({ nullptr, std::to_string(operand[1] && operand[0]) });
	else if (operation == "OR")
		intermediate.push_back({ nullptr, std::to_string(operand[1] || operand[0]) });
	else
		return true;

	return false;
}

//operation[0] - operation from back, so it's actually second operation then operation[1] - first operation
bool Expression::arithmeticOperationString(std::vector<std::tuple<std::shared_ptr<Component>, std::string> >& intermediate, std::string operand[2], std::string operation) {
	if (operation == "+")
		intermediate.push_back({ nullptr, operand[1] + operand[0] });
	else if (operation == "=")
		intermediate.push_back({ nullptr, std::to_string(operand[1] == operand[0]) });
	else if (operation == ">")
		intermediate.push_back({ nullptr, std::to_string(operand[1] > operand[0]) });
	else if (operation == "<")
		intermediate.push_back({ nullptr, std::to_string(operand[1] < operand[0]) });
	else
		return true;

	return false;
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