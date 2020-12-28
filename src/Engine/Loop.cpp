#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionComponents.h"
#include "InteractionSystems.h"
#include "InteractionActions.h"
#include "UserInputSystems.h"
#include "CustomComponents.h"

std::shared_ptr<Scene> activeScene;
std::shared_ptr<StateSystem> stateSystem;
std::shared_ptr<DrawSystem> drawSystem;
std::shared_ptr<InteractionSystem> interactionSystem;
std::shared_ptr<MouseSystem> mouseSystem;
std::shared_ptr<KeyboardSystem> keyboardSystem;

void surfaceCreated() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	std::shared_ptr<SceneManager> sceneManager = std::make_shared<SceneManager>();
	activeScene = sceneManager->createScene("basic");

	std::shared_ptr<TextureAtlas> atlas = TextureManager::instance()->createAtlas(GL_RGBA, { 4096, 4096 });
	std::shared_ptr<TextureRaw> textureRaw = TextureManager::instance()->createTexture("../data/textures/air_hockey_surface.png", atlas->getAtlasID(), { 0, 0 }, { 1, 1 });
	atlas->initialize();

	//1040 - 1103 for cyrillic: static_cast<int>(*(L"")))	
	GlyphsLoader::instance().initialize("../data/fonts/arial.ttf", { 1040, 1103 });
	GlyphsLoader::instance().bufferSymbols(15, 15);

	std::shared_ptr<Shader> shader = std::make_shared<Shader>("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	ShaderStore::instance()->addShader("texture", shader);
	{
		std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
		auto entity = activeScene->createEntity("Back");
		auto objectComponent = entity->createComponent<ObjectComponent>();
		objectComponent->initialize({ 300, 300 }, { 100, 100 }, bufferManager, ShaderStore::instance()->getShader("texture"));
		auto textureComponent = entity->createComponent<TextureComponent>();
		textureComponent->initialize(bufferManager);
		textureComponent->setTexture(textureRaw->getTextureID());
		auto mouseComponent = entity->createComponent<MouseComponent>();
		mouseComponent->initialize();
		auto clickInsideOp = std::make_shared<ExpressionOperation>();
		clickInsideOp->addArgument(entity, "", "");
		clickInsideOp->initializeOperation("CLICK ${0}");
		auto moveAct = std::make_shared<AssignAction>();
		moveAct->addArgument(entity, "ObjectComponent", "positionX");
		moveAct->initializeAction("${0} SET ${0} + 10");
		clickInsideOp->registerAction(moveAct);
		entity->createComponent<InteractionComponent>()->attachOperation(clickInsideOp, InteractionType::MOUSE_START);
	}
	{
		std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
		auto entity = activeScene->createEntity("Text");
		auto objectComponent = entity->createComponent<ObjectComponent>();
		objectComponent->initialize({ 300, 300 }, { 100, 100 }, bufferManager, ShaderStore::instance()->getShader("texture"));
		auto textComponent = entity->createComponent<TextComponent>();
		textComponent->initialize(bufferManager);
		textComponent->setText("Hello");
	}

	stateSystem = std::make_shared<StateSystem>();
	stateSystem->setEntityManager(activeScene->getEntityManager());
	interactionSystem = std::make_shared<InteractionSystem>();
	interactionSystem->setEntityManager(activeScene->getEntityManager());
	mouseSystem = std::make_shared<MouseSystem>();
	mouseSystem->setEntityManager(activeScene->getEntityManager());
	keyboardSystem = std::make_shared<KeyboardSystem>();
	keyboardSystem->setEntityManager(activeScene->getEntityManager());
	drawSystem = std::make_shared<DrawSystem>();
	drawSystem->setEntityManager(activeScene->getEntityManager());
}


void drawFrame() {
	//clear specified buffer's fields from previous draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Interaction should be called before graphic so we avoid use cases when sprite rendered in some coord but should be moved out due to some trigger	
	//so we observe "false" moves to coord under triger and instant move out	
	interactionSystem->update(InteractionType::COMMON_START);
	stateSystem->update(InteractionType::COMMON_START);
	mouseSystem->update(InteractionType::MOUSE_START);
	keyboardSystem->update(InteractionType::KEYBOARD_START);

	drawSystem->update();

	keyboardSystem->update(InteractionType::KEYBOARD_END);
	mouseSystem->update(InteractionType::MOUSE_END);
	stateSystem->update(InteractionType::COMMON_END);
	interactionSystem->update(InteractionType::COMMON_END);
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
	//glfwSwapInterval(0);
	//wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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