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
	_entity->createComponent<KeyboardComponent>();
	
	auto changeText = std::make_shared<ExpressionOperation>();
	changeText->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeText->addArgument(_entity->getComponent<TextComponent>(), "editable");
	changeText->initializeOperation("${0} = 1 AND ${1} = 1");
	auto editText = std::make_shared<AssignAction>();
	editText->addArgument(_entity->getComponent<KeyboardComponent>(), "symbol");
	editText->addArgument(_entity->getComponent<TextComponent>(), "text");
	editText->initializeAction("${1} SET ${1} + ${0}");
	changeText->registerAction(editText);
	_entity->createComponent<InteractionComponent>()->attachOperation(changeText, InteractionType::KEYBOARD_START);

	auto textOutOfBoundsDown = std::make_shared<ExpressionOperation>();
	textOutOfBoundsDown->addArgument(_entity->getComponent<ObjectComponent>(), "sizeY");
	textOutOfBoundsDown->addArgument(_entity->getComponent<TextComponent>(), "lineHeight");
	textOutOfBoundsDown->addArgument(_entity->getComponent<TextComponent>(), "page");
	textOutOfBoundsDown->addArgument(_entity->getComponent<TextComponent>(), "totalPages");
	textOutOfBoundsDown->addArgument(_entity->getComponent<TextComponent>(), "spacingCoeff");
	textOutOfBoundsDown->initializeOperation("( ${3} - ${2} ) * ${4} * ${1} > ${0}");

	auto changePageDown = std::make_shared<AssignAction>();
	changePageDown->addArgument(_entity->getComponent<TextComponent>(), "page");
	changePageDown->initializeAction("${0} SET ${0} + 1");
	textOutOfBoundsDown->registerAction(changePageDown);
	_entity->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsDown, InteractionType::KEYBOARD_END);

	auto textOutOfBoundsUp = std::make_shared<ExpressionOperation>();
	textOutOfBoundsUp->addArgument(_entity->getComponent<TextComponent>(), "page");
	textOutOfBoundsUp->addArgument(_entity->getComponent<TextComponent>(), "totalPages");
	textOutOfBoundsUp->initializeOperation("${1} - ${0} < 1");

	auto changePageUp = std::make_shared<AssignAction>();
	changePageUp->addArgument(_entity->getComponent<TextComponent>(), "page");
	changePageUp->initializeAction("${0} SET ${0} - 1");
	textOutOfBoundsUp->registerAction(changePageUp);
	_entity->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsUp, InteractionType::COMMON_START);

	auto clickInside = std::make_shared<ExpressionOperation>();
	clickInside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickX");
	clickInside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickY");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "positionX");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "positionY");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeX");
	clickInside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeY");
	//TODO: Add constants support to operations
	clickInside->initializeOperation("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5}");

	auto changeFocusOn = std::make_shared<AssignAction>();
	changeFocusOn->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOn->initializeAction("${0} SET 1");
	clickInside->registerAction(changeFocusOn);

	_entity->createComponent<InteractionComponent>()->attachOperation(clickInside, InteractionType::MOUSE_START);
	
	_entity->createComponent<MouseComponent>();
	auto clickOutside = std::make_shared<ExpressionOperation>();
	clickOutside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickX");
	clickOutside->addArgument(_entity->getComponent<MouseComponent>(), "leftClickY");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "positionX");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "positionY");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeX");
	clickOutside->addArgument(_entity->getComponent<ObjectComponent>(), "sizeY");
	//TODO: Add constants support to operations
	clickOutside->initializeOperation("${0} < ${2} OR ${0} > ${2} + ${4} OR ${1} < ${3} OR ${1} > ${3} + ${5}");

	auto changeFocusOff = std::make_shared<AssignAction>();
	changeFocusOff->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOff->initializeAction("${0} SET 0");
	clickOutside->registerAction(changeFocusOff);

	_entity->createComponent<InteractionComponent>()->attachOperation(clickOutside, InteractionType::MOUSE_START);
	
	return false;
}

bool Label::setEditable(bool editable) {
	_entity->getComponent<TextComponent>()->setMember("editable", editable);
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

