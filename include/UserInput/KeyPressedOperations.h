#pragma once
#include "UserInputEvents.h"
#include "Operation.h"

class KeyPressedAction : Action {

};

class KeyPressedOperation : public IKeyboardEvent, Operation {
	KeyPressedOperation();
	void keyboardPressed(int character, int action, int mode);
	~KeyPressedOperation();
};