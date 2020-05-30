#include "InteractionOperations.h"
#include <regex>
#include <assert.h>
#include "Common.h"
#include <sstream>

ExpressionOperation::ExpressionOperation(std::string name) {
	_name = name;
	_expression = std::make_shared<Expression>();
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

std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > ExpressionOperation::evaluateExpression(std::shared_ptr<ExpressionNode> node) {
	std::string value = node->getValue();
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > postfix;
	auto supportedOperations = _expression->getSupportedOperations();
	if (supportedOperations.find(value) != supportedOperations.end()) {
		auto childs = node->getNodes();

		//if this is operation we have to dive deeper to tree
		for (int i = childs.size() - 1; i >= 0; i--) {
			//from left to right
			auto result = evaluateExpression(childs[i]);
			for (auto item : result) {
				postfix.push_back(item);
			}
		}

		switch(childs.size()) {
			case 1: {
				auto oneArgumentResult = _expression->oneArgumentOperation(postfix.back(), value);
				if (std::get<1>(oneArgumentResult)) {
					postfix.pop_back();
					postfix.push_back({ nullptr, std::string(), std::get<0>(oneArgumentResult), -1 });
				}
				break;
			}
			case 2: {
				auto twoArgumentResult = _expression->twoArgumentOperation(postfix[postfix.size() - 1], postfix[postfix.size() - 2], value);
				if (std::get<3>(twoArgumentResult)) {
					postfix.pop_back();
					postfix.pop_back();
					postfix.push_back({ std::get<0>(twoArgumentResult), std::get<1>(twoArgumentResult), std::get<2>(twoArgumentResult) });
					continue;
				}
				break;
			}
			case 3: {
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
				break;
			}
			default: {
				//case with click to several entities
				break;
			}
		}
	}
	else {
		// Extraction of a sub-match
		std::regex varIndexRegex = std::regex("\\$\\{([\\d|\\,]+)\\}");
		std::smatch match;
		if (std::regex_search(value, match, varIndexRegex)) {
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
					std::string varName = std::get<2>(_arguments[varIndex]);
					postfix.push_back({ entity, componentName, varName, -1 });
				}
			}
		}
		else {
			postfix.push_back({ nullptr, "", value, -1 });
		}
	}
	//
	return postfix;
}

bool ExpressionOperation::checkOperation() {
	evaluateExpression(_postfix);
	/*
	auto supportedOperations = _expression->getSupportedOperations();
	std::vector<std::tuple<std::shared_ptr<OperationComponent>, std::string, int> > intermediate;
	std::vector<std::shared_ptr<Entity> > intermediateEntities;
	std::vector<std::shared_ptr<Entity> > intermediateBatchEntities;
	for (auto word = _postfix.begin(); word < _postfix.end(); word++) {
		//word (token) is operator
		if (supportedOperations.find(*word) != supportedOperations.end()) {
			//TODO: rewrite using information about arguments number
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
				continue;
			}
			
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
			assert("Can't find operation");
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
	*/
	return false;
}