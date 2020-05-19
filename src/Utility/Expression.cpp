#include "Expression.h"
#include <assert.h>
#include "UserInputComponents.h"
#include "GraphicComponents.h"
#include "Common.h"

Expression::Expression(std::map<std::string, std::tuple<int, std::string> > supportedOperations) {
	_supportedOperations = supportedOperations;
}

bool Expression::setCondition(std::string condition) {
	_condition = condition;
	return false;
}

std::string Expression::getCondition() {
	return _condition;
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

std::tuple<std::string, int> Expression::oneArgumentOperation(std::shared_ptr<Entity> entity, std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "CLICK" || 
		operation == "DOUBLE_CLICK") {
		auto mouseComponent = entity->getComponent<MouseComponent>();
		auto objectComponent = entity->getComponent<ObjectComponent>();
		if (!std::get<0>(entity->getComponent<ObjectComponent>()->getMemberFloat("visible"))) {
			result = { std::to_string(false), 1 };
		} else if (mouseComponent && objectComponent) {
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
	else
		result = { std::to_string(false), 0 };

	return result;
}

std::tuple<std::string, int> Expression::oneArgumentOperation(std::vector<std::shared_ptr<Entity> > batch, std::string operation) {
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
	} else
		result = { std::to_string(false), 0 };

	return result;
}

std::tuple<std::string, int> Expression::oneArgumentOperation(std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item, std::string operation) {
	std::tuple<std::string, int> result;
	if (operation == "SIZE") {
		auto vectorType = std::get<0>(item)->getVariableType(std::get<1>(item));
		if (vectorType == VariableType::varFloatVector) {
			int vectorSize = std::get<0>(std::get<0>(item)->getMemberVectorFloat(std::get<1>(item)))->size();
			result = { std::to_string(vectorSize), 1 };
		}
		else if (vectorType == VariableType::varStringVector) {
			int vectorSize = std::get<0>(std::get<0>(item)->getMemberVectorString(std::get<1>(item)))->size();
			result = { std::to_string(vectorSize), 1 };
		}
		else if (vectorType == VariableType::varUnknown) {
			result = { std::to_string(false), 0 };
		}
	}
	else if (operation == "!") {
		if (std::get<0>(item) == nullptr) {
			if (isNumber(std::get<1>(item)))
				result = { std::to_string(!stof(std::get<1>(item))), 1 };
		}
		else {
			result = { std::to_string(false), 0 };
		}
	}
	else
		result = { std::to_string(false), 0 };

	return result;
}

std::tuple<std::shared_ptr<OperationComponent>, std::string, int, int> Expression::twoArgumentOperation(std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item1, std::tuple<std::shared_ptr<OperationComponent>, std::string, int> item2, std::string operation) {
	std::tuple<std::shared_ptr<OperationComponent>, std::string, int, int> result;
	//item1 - second argument, item2 - first argument
	std::tuple<std::shared_ptr<OperationComponent>, std::string, int> operandTuple[2] = {item1, item2};
	VariableType operandType[2] = { VariableType::varUnknown, VariableType::varUnknown };
	bool operandConst[2] = { false, false };

	for (int i = 0; i < 2; i++) {
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

	std::tuple<float, bool> operandFloat[2] = { {0, false}, {0, false} };
	std::tuple<std::string, bool> operandString[2] = { {"", false}, {"", false} };
	for (int i = 0; i < 2; i++) {
		if (operandConst[i]) {
			if (operandType[i] == VariableType::varFloat)
				operandFloat[i] = { stof(std::get<1>(operandTuple[i])), true };
			else if (operandType[i] == VariableType::varString)
				operandString[i] = { std::get<1>(operandTuple[i]), true };
		}
		else if (operandType[i] == VariableType::varFloatVector) {
			auto operandValue = std::get<0>(operandTuple[i])->getMemberVectorFloat(std::get<1>(operandTuple[i]));
			if (std::get<1>(operandValue))
				operandFloat[i] = { std::get<0>(operandValue)->at(std::get<2>(operandTuple[i])), true };
			else
				result = {nullptr, std::to_string(false), 0, 0};
		}
		else if (operandType[i] == VariableType::varStringVector) {
			auto operandValue = std::get<0>(operandTuple[i])->getMemberVectorString(std::get<1>(operandTuple[i]));
			if (std::get<1>(operandValue)) {
				int index = std::get<2>(operandTuple[i]);
				if (index >= 0)
					operandString[i] = { std::get<0>(operandValue)->at(index), true };
			}
			else
				result = { nullptr, std::to_string(false), 0, 0 };
		}
		else if (operandType[i] == VariableType::varFloat) {
			auto operandValue = std::get<0>(operandTuple[i])->getMemberFloat(std::get<1>(operandTuple[i]));
			if (std::get<1>(operandValue))
				operandFloat[i] = { *std::get<0>(operandValue), true };
			else
				result = { nullptr, std::to_string(false), 0, 0 };
		}
		else if (operandType[i] == VariableType::varString) {
			auto operandValue = std::get<0>(operandTuple[i])->getMemberString(std::get<1>(operandTuple[i]));
			if (std::get<1>(operandValue))
				operandString[i] = { *std::get<0>(operandValue), true };
			else
				result = { nullptr, std::to_string(false), 0, 0 };
		}
	}

	if (operation == "AT") {
		//for AT second argument should be float
		if (std::get<1>(operandFloat[0])) {
			//push vector with correct index, index can only be float
			//keep component and field name, only change index and set it as second argument
			result = { std::get<0>(operandTuple[1]), std::get<1>(operandTuple[1]), std::get<0>(operandFloat[0]), 1 };
		} else 
			result = { nullptr, std::to_string(false), 0, 0 };
	}
	else if (operation == "SET") {
		//issues with accessing wrong members due to wrong operand's classification can happen
		//so need to check if member exist first

		//if second operand is float:
		//  if first operand is string so we need convert second operand to string
		//if second operand is string:
		//  if first operand is float so we need convert second operand to float
		if (std::get<1>(operandFloat[0])) {
			if (std::get<1>(operandString[1]))
				std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), std::to_string((int)std::get<0>(operandFloat[0])), std::get<2>(operandTuple[1]));
			else
				std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), std::get<0>(operandFloat[0]), std::get<2>(operandTuple[1]));
		}
		else if (std::get<1>(operandString[0])) {
			if (std::get<1>(operandFloat[1]))
				std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), stof(std::get<0>(operandString[0])), std::get<2>(operandTuple[1]));
			else
				std::get<0>(operandTuple[1])->setMember(std::get<1>(operandTuple[1]), std::get<0>(operandString[0]), std::get<2>(operandTuple[1]));
		}
		result = { nullptr, "1", -1, 1 };
	}
	else {
		//TODO: if operands are different
		if (std::get<1>(operandFloat[0]) && std::get<1>(operandFloat[1])) {
			float operand[2] = { std::get<0>(operandFloat[0]), std::get<0>(operandFloat[1]) };
			auto arithmeticResult = arithmeticOperationFloat(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
		else if (std::get<1>(operandString[0]) && std::get<1>(operandString[1])) {
			std::string operand[2] = { std::get<0>(operandString[0]), std::get<0>(operandString[1]) };
			auto arithmeticResult = arithmeticOperationString(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
		else {
			OUT_STREAM("WARNING: arithmetic operaton with different argument's type. Casting both to string.\n");
			std::string operand[2] = { std::to_string((int)std::get<0>(operandFloat[0])), std::get<0>(operandString[1]) };
			auto arithmeticResult = arithmeticOperationString(operand, operation);
			if (std::get<1>(arithmeticResult))
				result = { nullptr, std::get<0>(arithmeticResult), -1, 1 };
			else
				assert("Not defined arithmetic operation");
		}
	}

	return result;
}

bool Expression::prepareExpression(std::vector<std::string>& postfix) {
	//first we need to convert condition to postfix form
	//Shunting-yard algorithm
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
				postfix.push_back(operatorStack.back());
				operatorStack.pop_back();
			}
			operatorStack.push_back(*word);
		}
		else if (*word == "(") {
			operatorStack.push_back(*word);
		}
		else if (*word == ")") {
			while (operatorStack.back() != "(") {
				postfix.push_back(operatorStack.back());
				operatorStack.pop_back();
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
			postfix.push_back(*word);
		}
	}

	//pop remainings operators from the operator stack onto the output queue.
	while (operatorStack.size() > 0) {
		postfix.push_back(operatorStack.back());
		operatorStack.pop_back();
	}
}