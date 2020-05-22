#include "InteractionOperations.h"
#include <regex>
#include <assert.h>
#include "Common.h"
#include <sstream>
ExpressionOperation::ExpressionOperation(std::string name) {
	_name = name;
	_supportedOperations =
	{
		{ "AND",		  { 0, "left" } }, //last argument - the order sequence of the same operations should be executed in.
		{ "OR",			  { 0, "left" } }, //for example: 1 + 2 + 3 - can be processed from left to right BUT
		{ ">",			  { 1, "left" } }, // 2 ^ 2 ^ 3 - have to be processed from right to left
		{ "<",			  { 1, "left" } },
		{ "=",			  { 1, "left" } },
		{ "!",			  { 1, "left" } },
		{ "EMPTY",		  { 2, "left" } },
		{ "CLICK",		  { 2, "left" } },
		{ "DOUBLE_CLICK", { 2, "left" } },
		{ "+",			  { 2, "left" } }, 
		{ "-",			  { 2, "left" } },
		{ "/",			  { 3, "left" } },
		{ "*",			  { 3, "left" } },
		{ "^",			  { 4, "right"} },
		{ "SIZE",		  { 5, "left" } },
		{ "AT",			  { 5, "left" } }
	};
	_expression = std::make_shared<Expression>(_supportedOperations);
}

std::string ExpressionOperation::addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name) {
	int index = _arguments.size() + _views.size();
	_arguments[index] = { entity, component, name };
	return std::to_string(index);
}

std::string ExpressionOperation::addArgument(std::vector<std::shared_ptr<Entity> > entities) {
	std::string listIndexes = "";
	for (auto item = entities.begin(); item != entities.end(); item++) {
		listIndexes += addArgument(*item, "", "");
		if (std::next(item) != entities.end())
			listIndexes += ",";
	}
	return listIndexes;
}

bool ExpressionOperation::initializeOperation(std::string condition) {
	_expression->setCondition(condition);
	return _expression->prepareExpression(_postfix);
}

bool ExpressionOperation::checkOperation() {
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > intermediate;
	std::vector<std::shared_ptr<Entity> > intermediateEntities;
	std::vector<std::shared_ptr<Entity> > intermediateBatchEntities;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (_supportedOperations.find(*word) != _supportedOperations.end()) {
			if (intermediateEntities.size() > 0) {
				auto result = _expression->oneArgumentOperation(intermediateEntities.back(), *word);
				if (std::get<1>(result)) {
					intermediate.push_back({ nullptr, std::get<0>(result), -1 });
					intermediateEntities.pop_back();
					continue;
				}
			}
			if (intermediateBatchEntities.size() > 0) {
				auto result = _expression->oneArgumentOperation(intermediateBatchEntities, *word);
				if (std::get<1>(result)) {
					intermediate.push_back({ nullptr, std::get<0>(result), -1 });
					intermediateBatchEntities.clear();
					continue;
				}
			}

			if (intermediate.size() > 0) {
				auto oneArgumentResult = _expression->oneArgumentOperation(intermediate.back(), *word);
				if (std::get<1>(oneArgumentResult)) {
					intermediate.pop_back();
					intermediate.push_back({ nullptr, std::get<0>(oneArgumentResult), -1 });
					continue;
				}
			}

			auto twoArgumentResult = _expression->twoArgumentOperation(intermediate[intermediate.size() - 1], intermediate[intermediate.size() - 2], *word);
			if (std::get<3>(twoArgumentResult)) {
				intermediate.pop_back();
				intermediate.pop_back();
				intermediate.push_back({ std::get<0>(twoArgumentResult), std::get<1>(twoArgumentResult), std::get<2>(twoArgumentResult) });
			}
			else {
				assert("Can't find operation");
			}
			
		}
		//word (token) is operand
		else {
			// Extraction of a sub-match
			std::regex varIndexRegex = std::regex("\\$\\{([\\d|\\,]+)\\}");
			std::smatch match;
			if (std::regex_search(*word, match, varIndexRegex)) {
				std::string matchResult = match[1].str();
				std::stringstream ss(matchResult);
				std::vector<int> varIndexes;
				while (ss.good())
				{
					std::string substr;
					getline(ss, substr, ',');
					varIndexes.push_back(atoi(substr.c_str()));
				}
				for (auto varIndex : varIndexes) {
					//let's first check if record with such index exists in _arguments
					if (_arguments.find(varIndex) != _arguments.end()) {
						std::shared_ptr<Entity> entity = std::get<0>(_arguments[varIndex]);
						std::string componentName = std::get<1>(_arguments[varIndex]);
						std::shared_ptr<OperationComponent> component = nullptr;
						if (componentName != "")
							component = std::dynamic_pointer_cast<OperationComponent>(entity->getComponent(componentName));
						std::string varName = std::get<2>(_arguments[varIndex]);
						if (component == nullptr && entity != nullptr && varIndexes.size() == 1) {
							intermediateEntities.push_back(entity);
						}
						else if (component == nullptr && entity != nullptr && varIndexes.size() > 1) {
							intermediateBatchEntities.push_back(entity);
						}
						else
							intermediate.push_back({ component, varName, -1 });
					}
				}
			}
			else {
				intermediate.push_back({ nullptr, *word, -1 });
			}
		}
	}
	
	return stof(std::get<1>(intermediate.back()));
}
