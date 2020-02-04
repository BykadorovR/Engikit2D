#include "InteractionActions.h"
#include <assert.h>
#include <regex>

AssignAction::AssignAction() {
	_supportedOperations =
	{
		{ "SET", { 1, "left" } },
		{ "+",   { 2, "left" } },
		{ "-",   { 2, "left" } },
		{ "/",   { 3, "left" } },
		{ "*",   { 3, "left" } },
		{ "^",   { 4, "right"} }
	};
	_expression = std::make_shared<Expression>(_supportedOperations);
}

bool AssignAction::addArgument(std::shared_ptr<Component> argument, std::string name) {
	_arguments.push_back({ argument, name });
	return false;
}

bool AssignAction::addArgument(std::string name) {
	_arguments.push_back({ nullptr, name });
	return false;
}

bool AssignAction::setAction(std::string condition) {
	return _expression->setCondition(condition);
}

bool AssignAction::initializeAction() {
	return _expression->prepareExpression(_postfix);
}

bool AssignAction::doAction() {
	std::vector<float> intermidiate;
	std::tuple<std::shared_ptr<Component>, std::string> variableResult = { nullptr, "" };
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			float operand2 = intermidiate.back();
			intermidiate.pop_back();
			float operand1;
			if (intermidiate.size() > 0) {
				operand1 = intermidiate.back();
				intermidiate.pop_back();
			}
			
			if (*word == "+") {
				intermidiate.push_back(operand1 + operand2);
			}
			else if (*word == "-") {
				intermidiate.push_back(operand1 - operand2);
			}
			else if (*word == "*") {
				intermidiate.push_back(operand1 * operand2);
			}
			else if (*word == "/") {
				intermidiate.push_back(operand1 / operand2);
			}
			else if (*word == "^") {
				intermidiate.push_back(std::pow(operand1, operand2));
			}
			else if (*word == "SET") {
				std::get<0>(variableResult)->setMember(std::get<1>(variableResult), operand2);
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
				if (object) {
					auto value = object->getMember(varName);
					if (std::get<1>(value)) {
						if (std::get<0>(variableResult) == nullptr)
							variableResult = { object, varName };
						else
							intermidiate.push_back(*std::get<0>(value));
					}
					else {
						assert(0);
					}
				}
				else
					intermidiate.push_back(atoi(varName.c_str()));
			} else {
				intermidiate.push_back(atoi((*word).c_str()));
			}
		}
	}

	return false;
}
