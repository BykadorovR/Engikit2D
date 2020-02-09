#include "System.h"
#include "UserInputEvents.h"

class MouseSystem : public System, public IMouseEvent {
private:
	bool checkOperation();
public:
	MouseSystem();
	
	void mouseClickDownLeft(int x, int y);
	void mouseClickDownRight(int x, int y);

	~MouseSystem();
};