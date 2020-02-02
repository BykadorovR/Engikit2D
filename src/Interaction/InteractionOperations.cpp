#include "InteractionOperations.h"
#include <regex>
#include <assert.h>

ExpressionOperation::ExpressionOperation() {
	_supportedOperations =
	{
		{"AND", { 0, "left" } },
		{ "OR",  { 0, "left" } },
		{ ">",   { 1, "left" } },
		{ "<",   { 1, "left" } },
		{ "=",   { 1, "left" } },
		{ "+",   { 2, "left" } }, 
		{ "-",   { 2, "left" } },
		{ "/",   { 3, "left" } },
		{ "*",   { 3, "left" } },
		{ "^",   { 4, "right"} }
	};
	_expression = std::make_shared<Expression>(_supportedOperations);
}

bool ExpressionOperation::addArgument(std::shared_ptr<Component> argument, std::string name) {
	_arguments.push_back( { argument, name } );
	return false;
}

bool ExpressionOperation::setCondition(std::string condition) {
	return _expression->setCondition(condition);
}

bool ExpressionOperation::initializeOperation() {
	return _expression->prepareExpression(_postfix);
}

bool ExpressionOperation::checkOperation() {
	std::vector<float> result;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
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
					result.push_back(*std::get<0>(value));
				}
				else {
					assert(0);
				}
			}
			else {
				result.push_back(atoi((*word).c_str()));
			}
		}
	}
	
	return result.back();
}
