#include "Events.h"

void MouseEvent::registerComponent(std::shared_ptr<IMouseEvent> listener) {
	_listeners.push_back(listener);
}

void MouseEvent::mouseCursorPassive(int x, int y) {
	std::cout << "Passive " << x << " " << y << std::endl;
	for (auto listener : _listeners) {
		listener->mouseMove(x, y);
	}
}

void MouseEvent::mousePress(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		std::cout << "Left ";
		if (state == GLUT_DOWN) {
			std::cout << "Down " << x << " " << y << std::endl;
			for (auto listener : _listeners) {
				listener->mouseClickDownLeft(x, y);
			}
		}
	break;
	case GLUT_RIGHT_BUTTON:
		std::cout << "Right ";
		if (state == GLUT_DOWN) {
			std::cout << "Down " << x << " " << y << std::endl;
			for (auto listener : _listeners) {
				listener->mouseClickDownRight(x, y);
			}
		}
	break;
	}
}
