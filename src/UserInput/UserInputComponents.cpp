#include "UserInputComponents.h"

MouseComponent::MouseComponent() {
	_componentName = "MouseComponent";
	_classVariablesFloat =
	{
		{"leftClickX", &std::get<0>(_leftClick)},
		{"leftClickY", &std::get<1>(_leftClick)},
		{"rightClickX", &std::get<0>(_rightClick)},
		{"rightClickY", &std::get<1>(_rightClick)}
	};
}

KeyboardComponent::KeyboardComponent() {
	_componentName = "KeyboardComponent";
	_classVariablesFloat = 
	{
		{"code", &_code}
	};
	_classVariablesString = 
	{
		{"symbol", &_symbol}
	};
}
