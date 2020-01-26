#include "UserInputOperations.h"
#include <algorithm>

KeyInputOperation::KeyInputOperation() {
	KeyboardEvent::instance().registerComponent(this);
}

//actions should be registered in operation
void KeyInputOperation::keyboardPressed(int character, int action, int mode) {
	for (auto actionItem : _actions) {

	}
}

void KeyInputOperation::registerAction(Action* listener) {
	_actions.push_back(listener);
}

void KeyInputOperation::unregisterAction(Action* listener) {
	if (listener && _actions.size() > 0)
		_actions.erase(std::remove(_actions.begin(), _actions.end(), listener), _actions.end());
}

KeyInputOperation::~KeyInputOperation() {
	KeyboardEvent::instance().unregisterComponent(this);
}



MouseInputOperation::MouseInputOperation(std::tuple<float, float, float, float> area) {
	_area = area;
	MouseEvent::instance().registerComponent(this);
}

void MouseInputOperation::mouseClickDownLeft(int x, int y) {
	if (x > std::get<0>(_area) && x < std::get<2>(_area) &&
		y > std::get<1>(_area) && y < std::get<3>(_area)) {
		for (auto actionItem : _actions) {
		}
	}
}

void MouseInputOperation::mouseClickDownRight(int x, int y) {

}

MouseInputOperation::~MouseInputOperation() {
	MouseEvent::instance().unregisterComponent(this);
}

void MouseInputOperation::registerAction(Action* listener) {
	_actions.push_back(listener);
}

void MouseInputOperation::unregisterAction(Action* listener) {
	if (listener && _actions.size() > 0)
		_actions.erase(std::remove(_actions.begin(), _actions.end(), listener), _actions.end());
}