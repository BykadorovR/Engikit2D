#include "System.h"
#include "UserInputEvents.h"
#include "InteractionComponents.h"

class MouseSystem : public System, public IMouseEvent {
private:
	std::tuple<bool, bool> _needUpdate = { false, false };
public:
	MouseSystem();
	
	void mouseClickDownLeft(int x, int y);
	void mouseClickDownRight(int x, int y);

	void update(InteractionType type);

	~MouseSystem();
};

class KeyboardSystem : public System, public IKeyboardEvent {
private:
	std::tuple<bool, bool> _needUpdate = { false, false };
public:
	KeyboardSystem();

	void keyboardPressed(int key, int action, int mode);
	void textInput(unsigned int character);

	void update(InteractionType type);

	~KeyboardSystem();
};
