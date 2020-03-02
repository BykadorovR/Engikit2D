#include "InteractionActions.h"
#include <assert.h>
#include <regex>
#include "Common.h"

AssignAction::AssignAction() {
	_actionName = "AssignAction";
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

bool AssignAction::setAction(std::string condition) {
	return _expression->setCondition(condition);
}

bool AssignAction::initializeAction() {
	return _expression->prepareExpression(_postfix);
}

bool AssignAction::doAction() {
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > intermediate;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			std::tuple<std::shared_ptr<Component>, std::string> operandTuple[2];
			VariableType operandType[2] = { VariableType::varUnknown, VariableType::varUnknown };
			bool operandConst[2] = { false, false };

			for (int i = 0; i < 2; i++) {
				operandTuple[i] = intermediate.back();
				intermediate.pop_back();
				if (std::get<0>(operandTuple[i]) == nullptr) {
					operandConst[i] = true;
					if (isNumber(std::get<1>(operandTuple[i])))
						operandType[i] = VariableType::varFloat;
					else
						operandType[i] = VariableType::varString;
				}
				else {
					operandType[i] = std::get<0>(operandTuple[i])->getVariableType(std::get<1>(operandTuple[i]));
				}
			}

			if (operandType[0] == operandType[1] && operandType[1] == VariableType::varFloat) {
				float operand[2];
				for (int i = 0; i < 2; i++) {
					if (operandConst[i])
						operand[i] = stof(std::get<1>(operandTuple[i]));
					else {
						auto operandValue = std::get<0>(operandTuple[i])->getMemberFloat(std::get<1>(operandTuple[i]));
						if (std::get<1>(operandValue))
							operand[i] = *std::get<0>(operandValue);
						else
							return true;
					}
				}
				//operand[0] - second operand, operand[1] - first operand
				//operandTuple vice versa
				if (*word == "SET") {
					std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), operand[0]);
				} else
					_expression->arithmeticOperationFloat(intermediate, operand, *word);
			}
			else if (operandType[0] == operandType[1] && operandType[1] == VariableType::varString) {
				std::string operand[2];
				for (int i = 0; i < 2; i++) {
					if (operandConst[i])
						operand[i] = std::get<1>(operandTuple[i]);
					else {
						auto operandValue = std::get<0>(operandTuple[i])->getMemberString(std::get<1>(operandTuple[i]));
						if (std::get<1>(operandValue))
							operand[i] = *std::get<0>(operandValue);
						else
							return true;
					}
				}
				if (*word == "SET") {
					std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), operand[0]);
				}
				else
					_expression->arithmeticOperationString(intermediate, operand, *word);
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
				intermediate.push_back({ object, varName });
			}
			else {
				intermediate.push_back({ nullptr, *word });
			}
		}
	}

	return false;
}