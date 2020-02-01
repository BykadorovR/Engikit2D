#include "Expression.h"
#include <regex>

Expression::Expression() {
	_supportedOperations =
	{
		{">",   { 1, "left" } },
		{"<",   { 1, "left" } },
		{"=",   { 1, "left" } },
		{"AND", { 1, "left" } },
		{"OR",  { 1, "left" } },
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

bool Expression::calculateExpression() {
	//first we need to convert condition to postfix form
	//Shunting-yard algorithm
	std::string postfix;
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
		auto t = _supportedOperations.find(*word);
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			while (operatorStack.size() > 0 ||
				//operator at the top of the operator stack with greater precedence
				(std::get<0>(_supportedOperations[operatorStack.back()]) > std::get<0>(_supportedOperations[*word])) ||
				//operator at the top of the operator stack has equal precedence and the token is left associative
				(std::get<0>(_supportedOperations[operatorStack.back()]) == std::get<0>(_supportedOperations[*word]) && std::get<1>(_supportedOperations[*word]) == "left") ||
				//operator at the top of the operator stack is not a left parenthesis
				(operatorStack.back() != "(")) {
				//pop operators from the operator stack onto the output queue.
				postfix += operatorStack.back();
				operatorStack.pop_back();
			}
			operatorStack.push_back(*word);
		}
		else if (*word == "(") {
			operatorStack.push_back(*word);
		}
		else if (*word == ")") {
			while (operatorStack.back() != "(") {
				postfix += operatorStack.back();
				operatorStack.pop_back();
			}
			//discard
			if (operatorStack.back() == "(") {
				operatorStack.pop_back();
			}
		}
		//word (token) is number or variable
		else {
			postfix += *word;
		}
	}

	//pop remainings operators from the operator stack onto the output queue.
	while (operatorStack.size() > 0) {
		postfix += operatorStack.back();
		operatorStack.pop_back();
	}
	/*
	// Extraction of a sub-match
	const std::regex varIndexRegex("\\$\\{(\\d+)\\}");
	std::smatch match;
	if (std::regex_search(*word, match, varIndexRegex)) {
		std::string varIndex = match[1].str();
		postfix += std::get<1>(_arguments[atoi(varIndex.c_str())]);
	}
	else {
		postfix += *word;
	}
	*/
	return false;
}