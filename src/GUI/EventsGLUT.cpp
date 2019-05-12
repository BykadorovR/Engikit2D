#include "Events.h"
#include <algorithm>

void mousePress(GLFWwindow* window, int button, int action, int mods) {
	MouseEvent::instance().mousePress(window, button, action, mods);
}

void MouseEvent::registerComponent(IMouseEvent* listener) {
	_listeners.push_back(listener);
}

void MouseEvent::unregisterComponent(IMouseEvent* listener) {
	_listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
}

void MouseEvent::mousePress(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		std::cout << "Left ";
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			std::cout << "Down " << x << " " << y << std::endl;
			for (auto listener : _listeners) {
				listener->mouseClickDownLeft(x, y);
			}
		}
	break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		std::cout << "Right ";
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			std::cout << "Down " << x << " " << y << std::endl;
			for (auto listener : _listeners) {
				listener->mouseClickDownRight(x, y);
			}
		}
	break;
	}
}
