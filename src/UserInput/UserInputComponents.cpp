#include "UserInputComponents.h"

MouseComponent::MouseComponent() {
	_componentName = "MouseComponent";
	_classVariablesFloat =
	{
		{"leftClickX", {&std::get<0>(_leftClick), -1} },
		{"leftClickY", {&std::get<1>(_leftClick), -1} },
		{"rightClickX", {&std::get<0>(_rightClick), -1} },
		{"rightClickY", {&std::get<1>(_rightClick), -1} }
	};
}

KeyboardComponent::KeyboardComponent() {
	_componentName = "KeyboardComponent";
	_classVariablesString = 
	{
		{"symbol", {&_symbol, -1} }
	};
}
