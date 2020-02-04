#include "UserInputComponents.h"

MouseComponent::MouseComponent() {
	_componentName = "MouseComponent";
	_classVariables =
	{
		{"leftClickX", &std::get<0>(_leftClick)},
		{"leftClickY", &std::get<1>(_leftClick)},
		{"rightClickX", &std::get<0>(_rightClick)},
		{"rightClickY", &std::get<1>(_rightClick)}
	};
}