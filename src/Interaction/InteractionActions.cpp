#include "InteractionActions.h"
#include <assert.h>
#include <regex>
#include "Common.h"

AssignAction::AssignAction(std::string actionName) {
	_actionName = actionName;
	_expression = std::make_shared<Expression>();
	_expression->addSupportedOperation("SET",  { 1, "left" });
}

//TODO: separate common parts from operations and actions
bool AssignAction::addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name) {
	_arguments.push_back({ entity, component, name });
	return false;
}


bool AssignAction::initializeAction(std::string condition) {
	_expression->setCondition(condition);
	return _expression->prepareExpression(_postfix);
}

bool AssignAction::doAction() {
	auto supportedOperations = _expression->getSupportedOperations();
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > intermediate;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (supportedOperations.find(*word) != supportedOperations.end()) {
			//operations with only 1 argument
			if (intermediate.size() > 0) {
				auto oneArgumentResult = _expression->oneArgumentOperation(intermediate.back(), *word);
				if (std::get<1>(oneArgumentResult)) {
					intermediate.pop_back();
					intermediate.push_back({ nullptr, std::get<0>(oneArgumentResult), -1 });
					continue;
				}
			}
			
			if (intermediate.size() > 2) {
				auto threeArgumentResult = _expression->threeArgumentOperation(intermediate[intermediate.size() - 1],
					intermediate[intermediate.size() - 2],
					intermediate[intermediate.size() - 3],
					*word);
				if (std::get<3>(threeArgumentResult)) {
					intermediate.pop_back();
					intermediate.pop_back();
					intermediate.pop_back();
					intermediate.push_back({ std::get<0>(threeArgumentResult), std::get<1>(threeArgumentResult), std::get<2>(threeArgumentResult) });
					continue;
				}
			}

			if (intermediate.size() > 1) {
				//should be at the end because contains arithmetic operation (//TODO: parse operation for arithmetic operations too, so not just "else"
				auto twoArgumentResult = _expression->twoArgumentOperation(intermediate[intermediate.size() - 1], intermediate[intermediate.size() - 2], *word);
				if (std::get<3>(twoArgumentResult)) {
					intermediate.pop_back();
					intermediate.pop_back();
					intermediate.push_back({ std::get<0>(twoArgumentResult), std::get<1>(twoArgumentResult), std::get<2>(twoArgumentResult) });
					continue;
				}
			}
			
			assert("Can't find operation");

			//operand[0] - second operand, operand[1] - first operand
			//operandTuple vice versa
			//TODO: IMPL SET FOR ALL CASES!
			//if ${0} AT 2 SET 10
			//if ${0} SET ${1} AT 2
			//if ${0} AT 2 SET ${1} AT 2
		}
		//word (token) is operand
		else {
			// Extraction of a sub-match
			const std::regex varIndexRegex("\\$\\{(\\d+)\\}");
			std::smatch match;
			if (std::regex_search(*word, match, varIndexRegex)) {
				std::string varIndex = match[1].str();
				std::shared_ptr<Entity> entity = std::get<0>(_arguments[atoi(varIndex.c_str())]);
				std::string componentName = std::get<1>(_arguments[atoi(varIndex.c_str())]);
				std::shared_ptr<OperationComponent> component = nullptr;
				if (componentName != "")
					component = std::dynamic_pointer_cast<OperationComponent>(entity->getComponent(componentName));
				std::string varName = std::get<2>(_arguments[atoi(varIndex.c_str())]);
				intermediate.push_back({ component, varName, -1 });
			}
			else {
				intermediate.push_back({ nullptr, *word, -1 });
			}
		}
	}

	return false;
}