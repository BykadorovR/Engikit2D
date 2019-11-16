#include "UserInputEvents.h"

void mousePress(GLFWwindow* window, int button, int action, int mods) {
	MouseEvent::instance().mousePress(window, button, action, mods);
}

void keyboardPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	KeyboardEvent::instance().keyboardPressed(window, key, scancode, action, mods);
}

void KeyboardEvent::keyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void MouseEvent::mousePress(GLFWwindow* window, int button, int action, int mods) {

}

