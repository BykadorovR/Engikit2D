#include "UserInputComponents.h"

MouseComponent::MouseComponent() {
	MouseEvent::instance().registerComponent(this);
}

void MouseComponent::mouseClickDownLeft(int x, int y) {
	_leftClick = { x, y };
}

void MouseComponent::mouseClickDownRight(int x, int y) {
	_rightClick = { x, y };
}

std::tuple<float, float> MouseComponent::getLeftClick() {
	return _leftClick;
}

std::tuple<float, float> MouseComponent::getRightClick() {
	return _rightClick;
}

MouseComponent::~MouseComponent() {
	MouseEvent::instance().unregisterComponent(this);
}