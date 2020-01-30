#include "UserInputComponents.h"

MouseComponent::MouseComponent() {
	MouseEvent::instance().registerComponent(this);
	_classVariables = 
	{
		{"leftClickX", &std::get<0>(_leftClick)}, 
		{"leftClickY", &std::get<1>(_leftClick)},
		{"rightClickX", &std::get<0>(_rightClick)},
		{"rightClickY", &std::get<1>(_rightClick)}
	};
}

void MouseComponent::mouseClickDownLeft(int x, int y) {
	_leftClick = { x, y };
}

void MouseComponent::mouseClickDownRight(int x, int y) {
	_rightClick = { x, y };
}

MouseComponent::~MouseComponent() {
	MouseEvent::instance().unregisterComponent(this);
}