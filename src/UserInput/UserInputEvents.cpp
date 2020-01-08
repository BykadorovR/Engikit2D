#include "UserInputEvents.h"
#include "Common.h"
#include <algorithm>

void mousePress(GLFWwindow* window, int button, int action, int mods) {
	MouseEvent::instance().mousePress(window, button, action, mods);
}

void keyboardPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	KeyboardEvent::instance().keyboardPressed(window, key, scancode, action, mods);
}

void KeyboardEvent::registerComponent(IKeyboardEvent* listener) {
	_listeners.push_back(listener);
}

void KeyboardEvent::unregisterComponent(IKeyboardEvent* listener) {
	if (listener && _listeners.size() > 0)
		_listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
}

void KeyboardEvent::keyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (auto listener : _listeners) {
		listener->keyboardPressed(key, action, mods);
	}
}

void MouseEvent::registerComponent(IMouseEvent* listener) {
	_listeners.push_back(listener);
}

void MouseEvent::unregisterComponent(IMouseEvent* listener) {
	if (listener && _listeners.size() > 0)
		_listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
}

void MouseEvent::mousePress(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		OUT_STREAM("Left ");
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			OUT_STREAM("Down " << x << " " << y << std::endl);
			for (auto listener : _listeners) {
				listener->mouseClickDownLeft(x, y);
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		OUT_STREAM("Right ");
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			OUT_STREAM("Down " << x << " " << y << std::endl);
			for (auto listener : _listeners) {
				listener->mouseClickDownRight(x, y);
			}
		}
		break;
	}
}

