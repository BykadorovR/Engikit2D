#include "Label.h"
#include "Shader.h"
#include "UserInputComponents.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"

Label::Label(std::string name = "Label") {
	_viewName = name;
}

bool Label::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::string text, std::shared_ptr<GlyphsLoader> glyphs) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextComponent>()->initialize(text, glyphs, bufferManager);
	//TODO: how to pass texture here?

	_entity->createComponent<MouseComponent>();
	auto clickInside = std::make_shared<ExpressionOperation>();
	clickInside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickX");
	clickInside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickY");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "positionX");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "positionY");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeX");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeY");
	//TODO: Add constants support to operations
	clickInside->setCondition("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5}");
	clickInside->initializeOperation();

	auto changeFocusOn = std::make_shared<AssignAction>();
	changeFocusOn->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOn->setAction("${0} SET 1");
	changeFocusOn->initializeAction();
	clickInside->registerAction(changeFocusOn);

	_entity->createComponent<InteractionComponent>()->attachOperation(clickInside, InteractionType::MOUSE);
	
	_entity->createComponent<MouseComponent>();
	auto clickOutside = std::make_shared<ExpressionOperation>();
	clickOutside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickX");
	clickOutside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickY");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "positionX");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "positionY");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeX");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeY");
	//TODO: Add constants support to operations
	clickOutside->setCondition("${0} < ${2} OR ${0} > ${2} + ${4} OR ${1} < ${3} OR ${1} > ${3} + ${5}");
	clickOutside->initializeOperation();

	auto changeFocusOff = std::make_shared<AssignAction>();
	changeFocusOff->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOff->setAction("${0} SET 0");
	changeFocusOff->initializeAction();
	clickOutside->registerAction(changeFocusOff);

	_entity->createComponent<InteractionComponent>()->attachOperation(clickOutside, InteractionType::MOUSE);
	
	return false;
}

std::tuple<float, float> Label::getPosition() {
	return _entity->getComponent<ObjectComponent>()->getPosition();
}

std::tuple<float, float> Label::getSize() {
	return _entity->getComponent<ObjectComponent>()->getSize();
}

bool Label::setColor(std::vector<float> color) {
	_entity->getComponent<TextComponent>()->setColor(color);
	return false;
}

bool Label::setScale(float scale) {
	_entity->getComponent<TextComponent>()->setScale(scale);
	return false;
}

bool Label::setPageNumber(int pageNumber) {
	_entity->getComponent<TextComponent>()->setPageNumber(pageNumber);
	return false;
}

bool Label::setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment) {
	_entity->getComponent<TextComponent>()->setAllignment(allignment);
	return false;
}

bool Label::setLineSpacingCoeff(float coeff) {
	_entity->getComponent<TextComponent>()->setLineSpacingCoeff(coeff);
	return false;
}

LabelFactory::LabelFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> LabelFactory::createView(std::string name) {
	std::shared_ptr<Label> label = std::make_shared<Label>(name);
	label->setEntity(_activeScene->createEntity());
	label->getEntity()->createComponent<ObjectComponent>();
	label->getEntity()->createComponent<TextComponent>();
	return label;
}

