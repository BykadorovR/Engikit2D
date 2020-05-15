#include "InteractionOperations.h"
#include <regex>
#include <assert.h>
#include "Common.h"

ExpressionOperation::ExpressionOperation(std::string name) {
	_name = name;
	_supportedOperations =
	{
		{ "AND",	{ 0, "left" } }, //last argument - the order sequence of the same operations should be executed in.
		{ "OR",		{ 0, "left" } }, //for example: 1 + 2 + 3 - can be processed from left to right BUT
		{ ">",		{ 1, "left" } }, // 2 ^ 2 ^ 3 - have to be processed from right to left
		{ "<",		{ 1, "left" } },
		{ "=",		{ 1, "left" } },
		{ "!",      { 1, "left" } },
		{ "CLICK",  { 2, "left" } },
		{ "+",		{ 2, "left" } }, 
		{ "-",		{ 2, "left" } },
		{ "/",		{ 3, "left" } },
		{ "*",		{ 3, "left" } },
		{ "^",		{ 4, "right"} },
		{ "SIZE",   { 5, "left" } },
		{ "AT",     { 5, "left" } }
	};
	_expression = std::make_shared<Expression>(_supportedOperations);
}

bool ExpressionOperation::addArgument(std::shared_ptr<Entity> entity, std::string component, std::string name) {
	if (entity != nullptr && component == "" && name == "")
		_entities.push_back(entity);
	else
		_arguments.push_back( { entity, component, name } );

	return false;
}

bool ExpressionOperation::addArgument(std::shared_ptr<View> view) {
	_views.push_back(view);
	return false;
}

bool ExpressionOperation::initializeOperation(std::string condition) {
	_expression->setCondition(condition);
	return _expression->prepareExpression(_postfix);
}

bool ExpressionOperation::checkOperation() {
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > intermediate;
	std::vector<std::shared_ptr<Entity> > intermediateEntities;
	std::vector<std::shared_ptr<View> > intermediateViews;
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
			if (intermediateViews.size() > 0) {
				auto result = _expression->oneArgumentOperation(intermediateViews.back(), *word);
				if (std::get<1>(result)) {
					intermediate.push_back({ nullptr, std::get<0>(result), -1 });
					intermediateViews.pop_back();
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
			std::regex varIndexRegex("\\$\\{(\\d+)\\}");
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
				continue;
			}
			//find entity
			varIndexRegex = std::regex("\\#\\{(\\d+)\\}");
			if (std::regex_search(*word, match, varIndexRegex)) {
				std::string entityIndex = match[1].str();
				std::shared_ptr<Entity> currentEntity = _entities[atoi(entityIndex.c_str())];
				intermediateEntities.push_back(currentEntity);
				continue;
			}
			//find view
			varIndexRegex = std::regex("\\%\\{(\\d+)\\}");
			if (std::regex_search(*word, match, varIndexRegex)) {
				std::string entityIndex = match[1].str();
				std::shared_ptr<View> currentView = _views[atoi(entityIndex.c_str())];
				intermediateViews.push_back(currentView);
				continue;
			}
			
			intermediate.push_back({ nullptr, *word, -1 });
		}
	}
	
	return stof(std::get<1>(intermediate.back()));
}
