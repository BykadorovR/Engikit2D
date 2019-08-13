#include "General.h"
#include "World.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"
#include "Events.h"
#include "PlatformGL.h"
#include "Loop.h"
#include "Common.h"
#include "Events.h"

#include "Windows.h"
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	if (!glfwInit())
	{
		return -1;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile deprecate all fixed function API calls

	GLFWwindow* mainWindow;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	mainWindow = glfwCreateWindow(resolution.first, resolution.second, "Main window", glfwGetPrimaryMonitor(), NULL);	if (mainWindow == NULL) {
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

	on_surface_created();
	on_surface_changed();
	while (!glfwWindowShouldClose(mainWindow)) {
		// OpenGL API calls go here...

		on_draw_frame();
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	glfwDestroyWindow(mainWindow);
	glfwTerminate();	return 0;
}
