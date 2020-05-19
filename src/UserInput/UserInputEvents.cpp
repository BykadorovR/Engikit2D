#include "UserInputEvents.h"
#include "Common.h"
#include <algorithm>
#include <chrono>
void mousePressed(GLFWwindow* window, int button, int action, int mods) {
	MouseEvent::instance().mousePressed(window, button, action, mods);
}

void keyboardPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	KeyboardEvent::instance().keyboardPressed(window, key, scancode, action, mods);
}

void textInput(GLFWwindow* window, unsigned int codepoint) {
	KeyboardEvent::instance().textInput(window, codepoint);
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

void KeyboardEvent::textInput(GLFWwindow* window, unsigned int codepoint) {
	for (auto listener : _listeners) {
		listener->textInput(codepoint);
	}
}

void MouseEvent::registerComponent(IMouseEvent* listener) {
	_listeners.push_back(listener);
}

void MouseEvent::unregisterComponent(IMouseEvent* listener) {
	if (listener && _listeners.size() > 0)
		_listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
}

void MouseEvent::mousePressed(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			for (auto listener : _listeners) {
				listener->mouseClickDownLeft(x, y);
			}
		}
		else if (action == GLFW_RELEASE) {
			static auto firstClick = std::chrono::system_clock::now();
			auto secondClick = std::chrono::system_clock::now();
			double difference = std::chrono::duration <double, std::milli>(secondClick - firstClick).count();
			firstClick = secondClick;
			if (difference > 10 && difference < 300) {
				double x, y;
				//getting cursor position
				glfwGetCursorPos(window, &x, &y);
				for (auto listener : _listeners) {
					listener->mouseDoubleClickDownLeft(x, y);
				}
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			double x, y;
			//getting cursor position
			glfwGetCursorPos(window, &x, &y);
			for (auto listener : _listeners) {
				listener->mouseClickDownRight(x, y);
			}
		}
		break;
	}
}

