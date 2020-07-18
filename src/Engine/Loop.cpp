#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <chrono>
#include <thread>

#include "Interface.h"
#include "Grid.h"
#include "State.h"
#include "UserInputEvents.h"
#include "TextureAtlas.h"
#include "Texture.h"
#include "Entity.h"
#include "SceneManager.h"
#include "GraphicComponents.h"
#include "GraphicSystems.h"
#include "TextureManager.h"
#include "GlyphsLoader.h"
#include "Common.h"
#include "UIActions.h"

#include "Back.h"
#include "Label.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionComponents.h"
#include "InteractionSystems.h"
#include "InteractionActions.h"
#include "UserInputSystems.h"
#include "Decorator.h"
#include "List.h"
#include "CustomComponents.h"

std::shared_ptr<Scene> activeScene;
std::shared_ptr<StateSystem> stateSystem;
std::shared_ptr<DrawSystem> drawSystem;
std::shared_ptr<InteractionSystem> interactionSystem;
std::shared_ptr<MouseSystem> mouseSystem;
std::shared_ptr<KeyboardSystem> keyboardSystem;

void attachShowOperations(std::shared_ptr<Entity> entity, std::shared_ptr<List> list, std::shared_ptr<ScrollerVerticalDecorator> decorator) {
	auto printOperation = std::make_shared<ExpressionOperation>();
	printOperation->addArgument(entity, "", "");
	printOperation->initializeOperation("CLICK ${0}");
	auto printAction = std::make_shared<PrintOperationsAction>();
	printAction->setList(list);
	printAction->setEntity(entity);
	printOperation->registerAction(printAction);
	entity->createComponent<InteractionComponent>()->attachOperation(printOperation, InteractionType::MOUSE_START);
}

void attachShowComponents(std::shared_ptr<Entity> entity, std::shared_ptr<List> list, std::shared_ptr<ScrollerVerticalDecorator> decorator) {
	list->getViews()[0]->getEntity()->createComponent<CustomFloatComponent>()->addCustomValue("currentEntity", -1);
	list->getViews()[0]->getEntity()->createComponent<CustomFloatArrayComponent>()->initializeEmpty("registeredEntities");

	auto printComponentsOperation = std::make_shared<ExpressionOperation>();
	printComponentsOperation->addArgument(entity, "", "");
	printComponentsOperation->initializeOperation("CLICK ${0}");
	auto printComponentsAction = std::make_shared<PrintComponentsAction>();
	printComponentsAction->setList(list);
	printComponentsAction->setEntity(entity);
	printComponentsOperation->registerAction(printComponentsAction);
	entity->createComponent<InteractionComponent>()->attachOperation(printComponentsOperation, InteractionType::MOUSE_START);

	auto clearComponentsOperation = std::make_shared<ExpressionOperation>();
	clearComponentsOperation->addArgument(entity, "", "");
	std::string listIndexes = clearComponentsOperation->addArgument(list->getEntities());
	std::string decoratorIndexes = clearComponentsOperation->addArgument(decorator->getEntities());
	clearComponentsOperation->initializeOperation("! ( DOUBLE_CLICK ${0} ) AND ! ( CLICK ${"+listIndexes+"} ) AND ! ( CLICK ${"+decoratorIndexes+"} )");
	auto clearComponentsAction = std::make_shared<ClearComponentsAction>();
	clearComponentsAction->setList(list);
	clearComponentsAction->setEntity(entity);
	clearComponentsOperation->registerAction(clearComponentsAction);
	entity->createComponent<InteractionComponent>()->attachOperation(clearComponentsOperation, InteractionType::MOUSE_START);
}

void surfaceCreated() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}


void drawFrame() {
	//clear specified buffer's fields from previous draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	FreeConsole();
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile deprecate all fixed function API calls

	GLFWwindow* mainWindow;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//mainWindow = glfwCreateWindow(std::get<0>(currentResolution), std::get<1>(currentResolution), "Main window", monitor, NULL);
	mainWindow = glfwCreateWindow(std::get<0>(currentResolution), std::get<1>(currentResolution), "Main window", NULL, NULL);
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
	//disable VSync
	glfwSwapInterval(0);

	glfwSetMouseButtonCallback(mainWindow, mousePressed);
	glfwSetKeyCallback(mainWindow, keyboardPress);
	glfwSetCharCallback(mainWindow, textInput);
	//if size of window has changed we need to notify OpenGL that "working" area changed via glViewport
	glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height){ glViewport(0, 0, width, height); });

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	surfaceCreated();
	while (!glfwWindowShouldClose(mainWindow)) {
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		//TODO: separate display and bussness logic (display will be at 60 FPS, but business w/o restriction)
		//create buffer for rendering
		drawFrame();
		//double buffer - one for displaying, one for current render, once filled it's displayed. Needed to avoid flickering due to writing to displayed buffer
		glfwSwapBuffers(mainWindow);
		//checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions
		glfwPollEvents();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - startTime).count();
		OUT_STREAM("Elapsed: " + std::to_string(elapsed) + " ms\n");
	}

	glfwDestroyWindow(mainWindow);
	glfwTerminate();	
	return 0;
}