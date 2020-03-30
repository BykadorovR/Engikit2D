#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Windows.h"
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

#include "Back.h"
#include "Label.h"
#include "Button.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionComponents.h"
#include "InteractionSystems.h"
#include "InteractionActions.h"
#include "UserInputSystems.h"
#include "Decorator.h"

std::shared_ptr<Scene> activeScene;
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

	std::shared_ptr<GlyphsLoader> glyphsLoader = std::make_shared<GlyphsLoader>("../data/fonts/arial.ttf",
		std::make_tuple<int, int>(static_cast<int>(*(L"А")),
			static_cast<int>(*(L"я"))));
	glyphsLoader->bufferSymbols(24);

	//TODO: shaders should be global
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	ShaderStore::instance()->addShader("texture", shader);
	{
		std::shared_ptr<LabelFactory> labelFactory = std::make_shared<LabelFactory>(activeScene);
		std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(labelFactory->createView());
		label->initialize({ 50, 50 }, { 100, 100 }, "Hello  Привествую здесь всех на нашем корабле", glyphsLoader);

		std::shared_ptr<ScrollerDecoratorFactory> scrollerDecoratorFactory = std::make_shared<ScrollerDecoratorFactory>(activeScene);
		std::shared_ptr<ScrollerDecorator> scrollerDecorator = std::dynamic_pointer_cast<ScrollerDecorator>(scrollerDecoratorFactory->createView());
		scrollerDecorator->initialize(label);

		auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
		decoratorAddCheck->addArgument(label->getEntity()->getComponent<TextComponent>(), "totalPages");
		decoratorAddCheck->setCondition("${0} > 4");
		decoratorAddCheck->initializeOperation();

		auto decoratorAddAction = std::make_shared<LabelDecoratorDoAction>();
		decoratorAddAction->initializeAction(scrollerDecorator, activeScene);
		decoratorAddCheck->registerAction(decoratorAddAction);
		label->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::COMMON);

		std::shared_ptr<ButtonFactory> buttonFactory = std::make_shared<ButtonFactory>(activeScene);
		std::shared_ptr<Button> button = std::dynamic_pointer_cast<Button>(buttonFactory->createView());
		//TODO: rewrite to Back options and LabelOptions
		button->initialize({ 300, 200 }, { 100, 100 }, textureRaw->getTextureID(), "Жепа", {1, 0, 1, 1}, 1, glyphsLoader);
		button->getBack()->getEntity()->createComponent<KeyboardComponent>();

		auto changeText = std::make_shared<ExpressionOperation>();
		changeText->setCondition("1");
		changeText->initializeOperation();
		auto editText = std::make_shared<AssignAction>();
		editText->addArgument(button->getBack()->getEntity()->getComponent<KeyboardComponent>(), "symbol");
		editText->addArgument(button->getLabel()->getEntity()->getComponent<TextComponent>(), "text");
		editText->setAction("${1} SET ${1} + ${0}");
		editText->initializeAction();
		changeText->registerAction(editText);
		button->getBack()->getEntity()->createComponent<InteractionComponent>()->attachOperation(changeText, InteractionType::KEYBOARD);

		for (auto view : button->getViews()) {
			auto changePosition = std::make_shared<AssignAction>();
			changePosition->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "positionX");
			changePosition->setAction("${0} SET ${0} + 10");
			changePosition->initializeAction();
			button->addClickAction(changePosition);
		}

		auto boundCheck = std::make_shared<ExpressionOperation>();
		boundCheck->addArgument(button->getBack()->getEntity()->getComponent<ObjectComponent>(), "positionX");
		boundCheck->setCondition("${0} > 350");
		boundCheck->initializeOperation();

		for (auto view : button->getViews()) {
			auto returnBack = std::make_shared<AssignAction>();
			returnBack->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "positionX");
			returnBack->setAction("${0} SET 300");
			returnBack->initializeAction();
			boundCheck->registerAction(returnBack);
		}
		button->getBack()->getEntity()->createComponent<InteractionComponent>()->attachOperation(boundCheck, InteractionType::COMMON);

		button->getLabel()->setPageNumber(0);
		button->getLabel()->setLineSpacingCoeff(0.8);
		button->getLabel()->setTextAllignment({ TextAllignment::CENTER, TextAllignment::LEFT });
	}

	drawSystem = std::make_shared<DrawSystem>();
	drawSystem->setEntityManager(activeScene->getEntityManager());
	interactionSystem = std::make_shared<InteractionSystem>();
	interactionSystem->setEntityManager(activeScene->getEntityManager());
	mouseSystem = std::make_shared<MouseSystem>();
	mouseSystem->setEntityManager(activeScene->getEntityManager());
	keyboardSystem = std::make_shared<KeyboardSystem>();
	keyboardSystem->setEntityManager(activeScene->getEntityManager());

}


void drawFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Interaction should be called before graphic so we avoid use cases when sprite rendered in some coord but should be moved out due to some trigger
	//so we observe "false" moves to coord under triger and instant move out
	interactionSystem->update();

	drawSystem->update();
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

	glfwSetMouseButtonCallback(mainWindow, mousePressed);
	glfwSetKeyCallback(mainWindow, keyboardPress);
	glfwSetCharCallback(mainWindow, textInput);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	surfaceCreated();
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