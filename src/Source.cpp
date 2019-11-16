#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Windows.h"
#include "State.h"
#include "UserInputEvents.h"

void surfaceCreated() {

}

void surfaceChanged() {

}

void drawFrame() {
	
}

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	FreeConsole();
	if (!glfwInit())
	{
		return -1;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile deprecate all fixed function API calls

	GLFWwindow* mainWindow;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	mainWindow = glfwCreateWindow(std::get<0>(resolution), std::get<1>(resolution), "Main window", NULL /*monitor*/, NULL);
	if (mainWindow == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glfwSetMouseButtonCallback(mainWindow, mousePress);
	glfwSetKeyCallback(mainWindow, keyboardPress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	surfaceCreated();
	surfaceChanged();
	while (!glfwWindowShouldClose(mainWindow)) {
		// OpenGL API calls go here...

		drawFrame();
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	glfwDestroyWindow(mainWindow);
	glfwTerminate();	
	return 0;
}