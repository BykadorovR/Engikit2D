#include "InteractionOperations.h"
#include <regex>
#include <assert.h>
#include "Common.h"

ExpressionOperation::ExpressionOperation() {
	_supportedOperations =
	{
		{ "AND",	{ 0, "left" } }, //last argument - the order sequence of the same operations should be executed in.
		{ "OR",		{ 0, "left" } }, //for example: 1 + 2 + 3 - can be processed from left to right BUT
		{ ">",		{ 1, "left" } }, // 2 ^ 2 ^ 3 - have to be processed from right to left
		{ "<",		{ 1, "left" } },
		{ "=",		{ 1, "left" } },
		{ "CLICK",  { 1, "left" } },
		{ "+",		{ 2, "left" } }, 
		{ "-",		{ 2, "left" } },
		{ "/",		{ 3, "left" } },
		{ "*",		{ 3, "left" } },
		{ "^",		{ 4, "right"} }
	};
	_expression = std::make_shared<Expression>(_supportedOperations);
}

bool ExpressionOperation::addArgument(std::shared_ptr<OperationComponent> argument, std::string name, int index) {
	_arguments.push_back( { argument, name, index } );
	return false;
}

bool ExpressionOperation::addArgument(std::shared_ptr<Entity> entity) {
	_entities.push_back(entity);
	return false;
}

bool ExpressionOperation::initializeOperation(std::string condition) {
	_expression->setCondition(condition);
	return _expression->prepareExpression(_postfix);
}

bool ExpressionOperation::checkOperation() {
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > intermediate;
	std::vector<std::shared_ptr<Entity> > intermediateEntities;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			if (intermediateEntities.size() > 0 && !_expression->entityOperation(intermediate, intermediateEntities.back(), *word)) {
				intermediateEntities.pop_back();
				continue;
			}

			//arithmetic operations with component's fields
			std::tuple<std::shared_ptr<OperationComponent>, std::string, int> operandTuple[2];
			VariableType operandType[2] = {VariableType::varUnknown, VariableType::varUnknown};
			bool operandConst[2] = {false, false};

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
						auto operandValue = std::get<0>(operandTuple[i])->getMemberFloat(std::get<1>(operandTuple[i]),
																						 std::get<2>(operandTuple[i]));
						if (std::get<1>(operandValue))
							operand[i] = *std::get<0>(operandValue);
						else
							return true;
					}
				}
				_expression->arithmeticOperationFloat(intermediate, operand, *word);
			}
			else if (operandType[0] == operandType[1] && operandType[1] == VariableType::varString) {
				std::string operand[2];
				for (int i = 0; i < 2; i++) {
					if (operandConst[i])
						operand[i] = std::get<1>(operandTuple[i]);
					else {
						auto operandValue = std::get<0>(operandTuple[i])->getMemberString(std::get<1>(operandTuple[i]),
																						  std::get<2>(operandTuple[i]));
						if (std::get<1>(operandValue))
							operand[i] = *std::get<0>(operandValue);
						else
							return true;
					}
				}
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
				std::shared_ptr<OperationComponent> object = std::get<0>(_arguments[atoi(varIndex.c_str())]);
				std::string varName = std::get<1>(_arguments[atoi(varIndex.c_str())]);
				int index = std::get<2>(_arguments[atoi(varIndex.c_str())]);
				intermediate.push_back({ object, varName, index });
			}
			else {
				//find entity
				const std::regex varIndexRegex("\\#\\{(\\d+)\\}");
				std::smatch match;
				if (std::regex_search(*word, match, varIndexRegex)) {
					std::string entityIndex = match[1].str();
					std::shared_ptr<Entity> currentEntity = _entities[atoi(entityIndex.c_str())];
					intermediateEntities.push_back(currentEntity);
				}
				else {
					intermediate.push_back({ nullptr, *word, -1 });
				}
			}
		}
	}
	
	return stof(std::get<1>(intermediate.back()));
}
