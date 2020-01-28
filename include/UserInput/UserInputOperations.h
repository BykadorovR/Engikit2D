#pragma once
#include "UserInputEvents.h"
#include "Operation.h"
#include <tuple>

class KeyPressedAction : Action {

};

class KeyInputOperation : public IKeyboardEvent, public Operation {
	KeyInputOperation();
	void keyboardPressed(int character, int action, int mode);
	void registerAction(std::shared_ptr<Action> listener);
	void unregisterAction(std::shared_ptr<Action> listener);
	~KeyInputOperation();
};

class MouseClickAreaOperation : public IMouseEvent, public Operation {
private:
	std::tuple<float, float, float, float> _area;
public:
	MouseClickAreaOperation(std::tuple<float, float, float, float> area);
	void mouseClickDownLeft(int x, int y);
	void mouseClickDownRight(int x, int y);
	void registerAction(std::shared_ptr<Action> listener);
	void unregisterAction(std::shared_ptr<Action> listener);
	~MouseClickAreaOperation();
};