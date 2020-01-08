#include "KeyPressedOperations.h"

KeyPressedOperation::KeyPressedOperation() {
	KeyboardEvent::instance().registerComponent(this);
}

void KeyPressedOperation::keyboardPressed(int character, int action, int mode) {
	for (auto actionItem : _actions)
}

KeyPressedOperation::~KeyPressedOperation() {
	KeyboardEvent::instance().unregisterComponent(this);
}