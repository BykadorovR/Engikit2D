#include "Expression.h"
#include <regex>

Expression::Expression() {
	_supportedOperations =
	{
		{"AND", { 0, "left" } },
		{"OR",  { 0, "left" } },
		{">",   { 1, "left" } },
		{"<",   { 1, "left" } },
		{"=",   { 1, "left" } },
		{"+",   { 2, "left" } },
		{"-",   { 2, "left" } },
		{"/",   { 3, "left" } },
		{"*",   { 3, "left" } },
		{"^",   { 4, "right"} }
	};
}

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

bool Expression::calculateExpression(float* evaluatedResult) {
	//first we need to convert condition to postfix form
	//Shunting-yard algorithm
	std::vector<std::string> postfix;
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

	std::vector<float> result;
	for (auto word = postfix.begin(); word < postfix.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			float operand2 = result.back();
			result.pop_back();
			float operand1 = result.back();
			result.pop_back();
			if (*word == "+") {
				result.push_back(operand1 + operand2);
			}
			else if (*word == "-") {
				result.push_back(operand1 - operand2);
			}
			else if (*word == "*") {
				result.push_back(operand1 * operand2);
			}
			else if (*word == "/") {
				result.push_back(operand1 / operand2);
			}
			else if (*word == "^") {
				result.push_back(std::pow(operand1, operand2));
			}
			else if (*word == "=") {
				result.push_back(operand1 == operand2);
			}
			else if (*word == ">") {
				result.push_back(operand1 > operand2);
			}
			else if (*word == "<") {
				result.push_back(operand1 < operand2);
			}
			else if (*word == "AND") {
				result.push_back(operand1 && operand2);
			}
			else if (*word == "OR") {
				result.push_back(operand1 || operand2);
			}
		}
		//word (token) is operand
		else {
			// Extraction of a sub-match
			const std::regex varIndexRegex("\\$\\{(\\d+)\\}");
			std::smatch match;
			if (std::regex_search(*word, match, varIndexRegex)) {
				std::string varIndex = match[1].str();
				std::shared_ptr<Component> object = std::get<0>(_arguments[atoi(varIndex.c_str())]);
				std::string varName = std::get<1>(_arguments[atoi(varIndex.c_str())]);
				auto value = object->getMember(varName);
				if (std::get<1>(value)) {
					result.push_back(std::get<0>(value));
				}
				else {
					return true;
				}
			}
			else {
				result.push_back(atoi((*word).c_str()));
			}
		}
	}
	
	*evaluatedResult = result.back();
	return false;
}