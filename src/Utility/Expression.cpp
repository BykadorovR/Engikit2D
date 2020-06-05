#include "Expression.h"
#include <assert.h>
#include "UserInputComponents.h"
#include "GraphicComponents.h"
#include "Common.h"
#include <regex>
#include <algorithm>
#include <codecvt>

ExpressionNode::ExpressionNode(std::string value) {
	_value = value;
}

bool ExpressionNode::setNode(std::shared_ptr<ExpressionNode> node) {
	_nodes.push_back(node);
	return false;
}

std::string ExpressionNode::getValue() {
	return _value;
}

std::vector<std::shared_ptr<ExpressionNode> > ExpressionNode::getNodes() {
	return _nodes;
}

Expression::Expression() {
	_supportedOperations =
	{
		//TODO: if AND is found need to understand somehow that further processing is unneccessary!!

		{ "AND",		  { 0, "left", 2 } }, //last argument - the order sequence of the same operations should be executed in.
		{ "OR",			  { 0, "left", 2 } }, //for example: 1 + 2 + 3 - can be processed from left to right BUT
		{ ">",			  { 1, "left", 2 } }, // 2 ^ 2 ^ 3 - have to be processed from right to left
		{ "<",			  { 1, "left", 2 } },
		{ "=",			  { 1, "left", 2 } },
		{ "!",			  { 1, "left", 1 } },
		{ "INSERT",       { 1, "left", 3 } },
		{ "CLICK",		  { 2, "left", 1 } },
		{ "REMOVE",       { 2, "left", 2 } },
		{ "DOUBLE_CLICK", { 2, "left", 1 } },
		{ "+",			  { 2, "left", 2 } },
		{ "-",			  { 2, "left", 2 } },
		{ "/",			  { 3, "left", 2 } },
		{ "*",			  { 3, "left", 2 } },
		{ "^",			  { 4, "right",2 } },
		{ "SIZE",		  { 5, "left", 1 } },
		{ "AT",			  { 5, "left", 2 } }
	};
}

bool Expression::addSupportedOperation(std::string name, std::tuple<int, std::string, int> operation) {
	if (_supportedOperations.find(name) == _supportedOperations.end()) {
		_supportedOperations.insert({ name, operation });
		return false;
	}
	return true;
}

std::map<std::string, std::tuple<int, std::string, int> > Expression::getSupportedOperations() {
	return _supportedOperations;
}

bool Expression::addArgument(std::tuple<std::shared_ptr<Entity>, std::string, std::string> argument) {
	_arguments.push_back(argument);
	return false;
}

std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string> > Expression::getArguments() {
	return _arguments;
}

std::shared_ptr<ExpressionNode> Expression::getPostfix() {
	return _postfix;
}

bool Expression::prepareExpression(std::string condition) {
	//first we need to convert condition to postfix form
	//Shunting-yard algorithm
	std::vector<std::string> operatorStack;
	std::vector<std::string> splitedInput;
	//first of all we should split our operation by separator
	int wordIndex = 0;
	splitedInput.push_back("");
	for (auto c = condition.begin(); c < condition.end(); c++) {
		if (*c == *(" ")) {
			wordIndex++;
			splitedInput.push_back("");
			continue;
		}
		splitedInput[wordIndex] += *c;
	}

	std::vector<std::shared_ptr<ExpressionNode> > expressionTree;
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
				std::string operation = operatorStack.back();
				operatorStack.pop_back();

				std::shared_ptr<ExpressionNode> expressionOperation = std::make_shared<ExpressionNode>(operation);
				int numberParameters = std::get<2>(_supportedOperations[operation]);

				for (int i = 0; i < numberParameters; i++) {
					expressionOperation->setNode(expressionTree.back());
					expressionTree.pop_back();
				}
				expressionTree.push_back(expressionOperation);

			}
			operatorStack.push_back(*word);
		}
		else if (*word == "(") {
			operatorStack.push_back(*word);
		}
		else if (*word == ")") {
			while (operatorStack.back() != "(") {
				std::string operation = operatorStack.back();
				operatorStack.pop_back();

				std::shared_ptr<ExpressionNode> expressionOperation = std::make_shared<ExpressionNode>(operation);
				int numberParameters = std::get<2>(_supportedOperations[operation]);

				for (int i = 0; i < numberParameters; i++) {
					expressionOperation->setNode(expressionTree.back());
					expressionTree.pop_back();
				}
				expressionTree.push_back(expressionOperation);
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
			expressionTree.push_back(std::make_shared<ExpressionNode>(*word));
		}
	}

	//pop remainings operators from the operator stack onto the output queue.
	while (operatorStack.size() > 0) {
		std::string operation = operatorStack.back();
		operatorStack.pop_back();

		std::shared_ptr<ExpressionNode> expressionOperation = std::make_shared<ExpressionNode>(operation);
		int numberParameters = std::get<2>(_supportedOperations[operation]);

		for (int i = 0; i < numberParameters; i++) {
			expressionOperation->setNode(expressionTree.back());
			expressionTree.pop_back();
		}
		expressionTree.push_back(expressionOperation);
	}

	_postfix = expressionTree.back();
	return false;
}

std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > Expression::evaluateExpression(std::shared_ptr<ExpressionNode> node) {
	std::string value = node->getValue();
	std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > postfix;
	if (_supportedOperations.find(value) != _supportedOperations.end()) {
		auto childs = node->getNodes();
		//if this is operation we have to dive deeper to tree
		for (int i = childs.size() - 1; i >= 0; i--) {
			//from left to right
			auto result = evaluateExpression(childs[i]);
			//short circuit evaluation
			if (value == "AND") {
				if (stof(std::get<2>(result.back())) == 0) {
					//we shouldn't check right argument
					postfix.clear();
					postfix.push_back({ nullptr, std::string(), std::to_string(false), -1 });
					return postfix;
				}
			}
			else if (value == "OR") {
				if (stof(std::get<2>(result.back())) == 1) {
					//we shouldn't check right argument
					postfix.clear();
					postfix.push_back({ nullptr, std::string(), std::to_string(true), -1 });
					return postfix;
				}
			}
			for (auto item : result) {
				postfix.push_back(item);
			}
		}
		//this can happen only if we have ${1,2,3,4} argument so it's parsed as 1 argument but in reality it's 4 arguments
		if (postfix.size() != childs.size()) {
			auto multipleArgumentResult = multipleArgumentOperation(postfix, value);
			if (std::get<1>(multipleArgumentResult)) {
				postfix.clear();
				postfix.push_back({ nullptr, std::string(), std::get<0>(multipleArgumentResult), -1 });
			}
		}
		else {
			switch (postfix.size()) {
				case 1: {
					auto oneArgumentResult = oneArgumentOperation(postfix.back(), value);	
					if (std::get<1>(oneArgumentResult)) {
						postfix.pop_back();
						postfix.push_back({ nullptr, std::string(), std::get<0>(oneArgumentResult), -1 });
					}
					break;
				}
				case 2: {
					auto twoArgumentResult = twoArgumentOperation(postfix[postfix.size() - 1],
																  postfix[postfix.size() - 2],
																  value);
					if (std::get<4>(twoArgumentResult)) {
						postfix.pop_back();
						postfix.pop_back();
						postfix.push_back({ std::get<0>(twoArgumentResult), std::get<1>(twoArgumentResult), std::get<2>(twoArgumentResult), std::get<3>(twoArgumentResult) });
					}
					break;
				}
				case 3: {
					auto threeArgumentResult = threeArgumentOperation(postfix[postfix.size() - 1],
																	  postfix[postfix.size() - 2],
																	  postfix[postfix.size() - 3],
																	  value);
					if (std::get<4>(threeArgumentResult)) {
						postfix.pop_back();
						postfix.pop_back();
						postfix.pop_back();
						postfix.push_back({ std::get<0>(threeArgumentResult), std::get<1>(threeArgumentResult), std::get<2>(threeArgumentResult), std::get<3>(threeArgumentResult) });
					}
					break;
				}
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
				if (varIndex < _arguments.size()) {
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

std::tuple<std::string, int> Expression::oneArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item,
	std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "CLICK" ||
		operation == "DOUBLE_CLICK") {
		auto entity = std::get<0>(item);
		auto mouseComponent = entity->getComponent<MouseComponent>();
		auto objectComponent = entity->getComponent<ObjectComponent>();
		if (!std::get<0>(entity->getComponent<ObjectComponent>()->getMemberFloat("visible"))) {
			result = { std::to_string(false), 1 };
		}
		else if (mouseComponent && objectComponent) {
			float x = std::get<0>(objectComponent->getPosition());
			float y = std::get<1>(objectComponent->getPosition());
			float width = std::get<0>(objectComponent->getSize());
			float height = std::get<1>(objectComponent->getSize());
			auto clickX = mouseComponent->getMemberFloat("leftClickX");
			auto clickY = mouseComponent->getMemberFloat("leftClickY");
			if (!std::get<1>(clickX) || !std::get<1>(clickY))
				result = { std::to_string(false), 1 };

			if (*std::get<0>(clickX) > x && *std::get<0>(clickX) < x + width && *std::get<0>(clickY) > y && *std::get<0>(clickY) < y + height) {
				result = { std::to_string(true), 1 };
			}
			else {
				result = { std::to_string(false), 1 };
			}
			auto doubleClick = mouseComponent->getMemberFloat("doubleClick");
			if (operation == "DOUBLE_CLICK" && (bool)*std::get<0>(doubleClick) == false)
				result = { std::to_string(false), 1 };
		}
		else {
			//set false as result
			result = { std::to_string(false), 1 };
		}
	}
	else if (operation == "SIZE") {
		std::shared_ptr<OperationComponent> component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(item)->getComponent(std::get<1>(item)));
		auto vectorType = component->getVariableType(std::get<2>(item));
		if (vectorType == VariableType::varFloatVector) {
			int vectorSize = std::get<0>(component->getMemberVectorFloat(std::get<2>(item)))->size();
			result = { std::to_string(vectorSize), 1 };
		}
		else if (vectorType == VariableType::varStringVector) {
			int vectorSize = std::get<0>(component->getMemberVectorString(std::get<2>(item)))->size();
			result = { std::to_string(vectorSize), 1 };
		}
		else if (vectorType == VariableType::varString) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring text = converter.from_bytes(*std::get<0>(component->getMemberString(std::get<2>(item))));
			result = { std::to_string(text.size()), 1 };
		}
		else if (vectorType == VariableType::varUnknown) {
			result = { std::to_string(false), 0 };
		}
	}
	else if (operation == "!") {
		if (std::get<1>(item).empty()) {
			if (isNumber(std::get<2>(item)))
				result = { std::to_string(!stof(std::get<2>(item))), 1 };
		}
		else {
			result = { std::to_string(false), 0 };
		}
	}
	else
		result = { std::to_string(false), 0 };

	return result;
}

std::tuple<std::shared_ptr<Entity>, std::string, std::string, int, int> Expression::twoArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int>  item1,
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int>  item2,
	std::string operation) {
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int, int> result;
	//TODO: do correct order
	//item1 - second argument, item2 - first argument
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> operandTuple[2] = { item1, item2 };
	VariableType operandType[2] = { VariableType::varUnknown, VariableType::varUnknown };
	bool operandConst[2] = { false, false };

	for (int i = 0; i < 2; i++) {
		if (std::get<1>(operandTuple[i]).empty()) {
			operandConst[i] = true;
			if (isNumber(std::get<2>(operandTuple[i])))
				operandType[i] = VariableType::varFloat;
			else
				operandType[i] = VariableType::varString;
		}
		else {
			std::shared_ptr<OperationComponent> component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(operandTuple[i])->getComponent(std::get<1>(operandTuple[i])));
			operandType[i] = component->getVariableType(std::get<2>(operandTuple[i]));
		}
	}

	std::tuple<float, bool> operandFloat[2] = { {0, false}, {0, false} };
	std::tuple<std::string, bool> operandString[2] = { {"", false}, {"", false} };
	for (int i = 0; i < 2; i++) {
		if (operandConst[i]) {
			if (operandType[i] == VariableType::varFloat)
				operandFloat[i] = { stof(std::get<2>(operandTuple[i])), true };
			else if (operandType[i] == VariableType::varString)
				operandString[i] = { std::get<2>(operandTuple[i]), true };
		}
		else {
			std::shared_ptr<OperationComponent> component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(operandTuple[i])->getComponent(std::get<1>(operandTuple[i])));
			if (operandType[i] == VariableType::varFloatVector) {
				auto operandValue = component->getMemberVectorFloat(std::get<2>(operandTuple[i]));
				if (std::get<1>(operandValue))
					operandFloat[i] = { std::get<0>(operandValue)->at(std::get<3>(operandTuple[i])), true };
				else
					result = { nullptr, std::string(), std::string(), -1, 0 };
			}
			else if (operandType[i] == VariableType::varStringVector) {
				auto operandValue = component->getMemberVectorString(std::get<2>(operandTuple[i]));
				if (std::get<1>(operandValue)) {
					int index = std::get<3>(operandTuple[i]);
					if (index >= 0)
						operandString[i] = { std::get<0>(operandValue)->at(index), true };
				}
				else
					result = { nullptr, std::string(), std::string(), -1, 0 };
			}
			else if (operandType[i] == VariableType::varFloat) {
				auto operandValue = component->getMemberFloat(std::get<2>(operandTuple[i]));
				if (std::get<1>(operandValue))
					operandFloat[i] = { *std::get<0>(operandValue), true };
				else
					result = { nullptr, std::string(), std::string(), -1, 0 };
			}
			else if (operandType[i] == VariableType::varString) {
				auto operandValue = component->getMemberString(std::get<2>(operandTuple[i]));
				if (std::get<1>(operandValue)) {
					int index = std::get<3>(operandTuple[i]);
					if (index >= 0) {
						operandString[i] = { std::string(1, std::get<0>(operandValue)->at(index)), true };
					}
					else
						operandString[i] = { *std::get<0>(operandValue), true };
				}
				else
					result = { nullptr, std::string(), std::string(), -1, 0 };
			}
		}
	}

	if (operation == "AT") {
		//for AT second argument should be float
		if (std::get<1>(operandFloat[0])) {
			//push vector with correct index, index can only be float
			//keep component and field name, only change index and set it as second argument
			result = { std::get<0>(operandTuple[1]), std::get<1>(operandTuple[1]), std::get<2>(operandTuple[1]), std::get<0>(operandFloat[0]), 1 };
		}
		else
			result = { nullptr, std::string(), std::string(), -1, 0 };
	}
	else if (operation == "SET") {
		//issues with accessing wrong members due to wrong operand's classification can happen
		//so need to check if member exist first

		//if second operand is float:
		//  if first operand is string so we need convert second operand to string
		//if second operand is string:
		//  if first operand is float so we need convert second operand to float
		std::shared_ptr<OperationComponent> component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(operandTuple[1])->getComponent(std::get<1>(operandTuple[1])));
		if (std::get<1>(operandFloat[0])) {
			if (std::get<1>(operandString[1]))
				component->setMember(std::get<2>(operandTuple[1]), std::to_string((int)std::get<0>(operandFloat[0])), std::get<3>(operandTuple[1]));
			else
				component->setMember(std::get<2>(operandTuple[1]), std::get<0>(operandFloat[0]), std::get<3>(operandTuple[1]));
		}
		else if (std::get<1>(operandString[0])) {
			if (std::get<1>(operandFloat[1]))
				component->setMember(std::get<2>(operandTuple[1]), stof(std::get<0>(operandString[0])), std::get<3>(operandTuple[1]));
			else
				component->setMember(std::get<2>(operandTuple[1]), std::get<0>(operandString[0]), std::get<3>(operandTuple[1]));
		}
		result = { nullptr, std::string(), std::to_string(1), -1, 1 };
	}
	else if (operation == "REMOVE") {
		if (!std::get<1>(operandFloat[0]))
			assert("index is not float");

		std::shared_ptr<OperationComponent> component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(operandTuple[1])->getComponent(std::get<1>(operandTuple[1])));
		int index = std::get<0>(operandFloat[0]);
		if (operandType[1] == VariableType::varString) {
			auto targetString = std::get<0>(component->getMemberString(std::get<2>(operandTuple[1])));
			targetString->erase(targetString->begin() + index - 1);
		}
		else if (VariableType::varStringVector) {
			auto targetStringVector = std::get<0>(component->getMemberVectorString(std::get<2>(operandTuple[1])));
			targetStringVector->erase(targetStringVector->begin() + index - 1);
		}
		else if (VariableType::varFloatVector) {
			auto targetFloatVector = std::get<0>(component->getMemberVectorFloat(std::get<1>(operandTuple[1])));
			targetFloatVector->erase(targetFloatVector->begin() + index - 1);
		}
		else {
			assert("not implemented");
		}
		result = { nullptr, std::string(), std::to_string(1), -1, 1 };
	}
	else {
		//TODO: if operands are different
		if (std::get<1>(operandFloat[0]) && std::get<1>(operandFloat[1])) {
			float operand[2] = { std::get<0>(operandFloat[0]), std::get<0>(operandFloat[1]) };
			auto arithmeticResult = arithmeticOperationFloat(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::string(), std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
		else if (std::get<1>(operandString[0]) && std::get<1>(operandString[1])) {
			std::string operand[2] = { std::get<0>(operandString[0]), std::get<0>(operandString[1]) };
			auto arithmeticResult = arithmeticOperationString(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::string(), std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
		else {
			OUT_STREAM("WARNING: arithmetic operaton with different argument's type. Casting both to string.\n");
			std::string operand[2];
			if (operandType[1] == VariableType::varString) {
				//can contain any amount of symbols
				operand[1] = std::get<0>(operandString[1]);

				//interpret second argument as 1 symbol
				char symbol = (char)std::get<0>(operandFloat[0]);
				//compare if values are equal so we don't lose info due to conversion
				if (symbol == std::get<0>(operandFloat[0]))
					operand[0] = std::string(1, symbol);
				else
					//otherwise convert it to string
					operand[0] = std::to_string(std::get<0>(operandFloat[0]));
			}
			else {
				assert("Not implemented");
			}

			auto arithmeticResult = arithmeticOperationString(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::string(), std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
	}

	return result;
}

std::tuple<std::shared_ptr<Entity>, std::string, std::string, int, int> Expression::threeArgumentOperation(std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item1,
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item2,
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> item3,
	std::string operation) {
	std::tuple<std::shared_ptr<Entity>, std::string, std::string, int, int> result;
	//item3 - container we wanna insert to
	//item2 - value
	//item1 - position
	//TODO: use approach from two arguments to determine type/const/etc
	if (operation == "INSERT") {
		std::shared_ptr<OperationComponent> item1Component = nullptr;
		if (std::get<0>(item1))
			item1Component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(item1)->getComponent(std::get<1>(item1)));
		int index;
		//index can be const
		if (item1Component == nullptr)
			//Suppose index is float
			index = atoi(std::get<2>(item1).c_str());
		else {
			auto type = item1Component->getVariableType(std::get<2>(item1));
			if (type == VariableType::varFloat)
				index = *std::get<0>(item1Component->getMemberFloat(std::get<2>(item1)));
			else if (type == VariableType::varFloatVector)
				index = std::get<0>(item1Component->getMemberVectorFloat(std::get<2>(item1)))->at(std::get<3>(item1));
			else
				assert("Not implemented");
		}
		auto item3Component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(item3)->getComponent(std::get<1>(item3)));
		auto containerType = item3Component->getVariableType(std::get<2>(item3));
		if (containerType == VariableType::varString) {
			std::shared_ptr<OperationComponent> item2Component = nullptr;
			if (std::get<0>(item2))
				item2Component = std::dynamic_pointer_cast<OperationComponent>(std::get<0>(item2)->getComponent(std::get<1>(item2)));
			std::string value;
			if (item2Component == nullptr)
				value = std::get<2>(item2);
			else {
				value = *std::get<0>(item2Component->getMemberString(std::get<2>(item2)));
			}
			if (value == "\n") {
				value = std::string(1, '\n');
			}
			auto targetString = *std::get<0>(item3Component->getMemberString(std::get<2>(item3)));
			//convert string to wstring in case of non-English symbols
			//it won't affect English/digitals
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring text = converter.from_bytes(targetString);
			text.insert(index, converter.from_bytes(value));
			//convert it back
			std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converterBack;
			item3Component->setMember(std::get<2>(item3), converterBack.to_bytes(text));
		}
		else
			assert("Not implemented");
		result = { nullptr, std::string(), std::to_string(true), -1, 1 };
	}
	else {
		result = { nullptr, std::string(), std::to_string(false), -1, 0 };
	}

	return result;
}

std::tuple<std::string, int> Expression::multipleArgumentOperation(std::vector<std::tuple<std::shared_ptr<Entity>, std::string, std::string, int> > batch, std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "CLICK") {
		bool anyClick = false;
		for (auto entity : batch) {
			if (std::get<1>(oneArgumentOperation(entity, operation)) && atoi(std::get<0>(oneArgumentOperation(entity, operation)).c_str())) {
				anyClick = true;
				break;
			}
		}
		if (anyClick)
			result = { std::to_string(true), 1 };
		else
			result = { std::to_string(false), 1 };
	}
	else
		result = { std::to_string(false), 0 };

	return result;
}

//operation[0] - operation from back, so it's actually second operation then operation[1] - first operation
std::tuple<std::string, int> Expression::arithmeticOperationFloat(float operand[2], std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "+")
		result = { std::to_string(operand[1] + operand[0]), 1 };
	else if (operation == "-")
		result = { std::to_string(operand[1] - operand[0]), 1 };
	else if (operation == "*")
		result = { std::to_string(operand[1] * operand[0]), 1 };
	else if (operation == "/") {
		assert(operand[0] != 0, "Division by zero");
		result = { std::to_string(operand[1] / operand[0]), 1 };
	}
	else if (operation == "=")
		result = { std::to_string(operand[1] == operand[0]), 1 };
	else if (operation == ">")
		result = { std::to_string(operand[1] > operand[0]), 1 };
	else if (operation == "<")
		result = { std::to_string(operand[1] < operand[0]), 1 };
	else if (operation == "AND")
		result = { std::to_string(operand[1] && operand[0]), 1 };
	else if (operation == "OR")
		result = { std::to_string(operand[1] || operand[0]), 1 };
	else
		result = { "", 0 };

	return result;
}

//operation[0] - operation from back, so it's actually second operation then operation[1] - first operation
std::tuple<std::string, int> Expression::arithmeticOperationString(std::string operand[2], std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "+")
		result = { (operand[1] + operand[0]), 1 };
	else if (operation == "-") {
		std::string tmp = operand[1];
		int removeNumber = *(operand[0].c_str());
		tmp.erase(tmp.end() - removeNumber, tmp.end());
		result = { tmp , 1 };
	}
	else if (operation == "=")
		result = { std::to_string(operand[1] == operand[0]), 1 };
	else if (operation == ">")
		result = { std::to_string(operand[1] > operand[0]), 1 };
	else if (operation == "<")
		result = { std::to_string(operand[1] < operand[0]), 1 };
	else
		result = { "", 0 };

	return result;
}