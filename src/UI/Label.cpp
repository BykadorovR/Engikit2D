#include "Label.h"
#include "Shader.h"
#include "State.h"
#include "UserInputComponents.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "UIActions.h"

Label::Label(std::string name = "Label") {
	_viewName = name;
}

/*
List of operation-actions:
1) if "focus && editable" then "add symbol to text"
2) if "clicked inside" then "set focus"
3) if "clicked outside" then "unset focus"
*/
bool Label::initialize() {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize({ std::get<0>(resolution) / 2, std::get<1>(resolution) / 2}, {100, 100}, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextComponent>()->initialize(bufferManager);

	_entity->createComponent<MouseComponent>();
	_entity->createComponent<KeyboardComponent>();

	//--- 1
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
	//--- 1

	//--- 2
	auto clickInside = std::make_shared<ExpressionOperation>();
	clickInside->addArgument(_entity);
	//TODO: Add constants support to operations
	clickInside->initializeOperation("CLICK #{0}");
	auto changeFocusOn = std::make_shared<AssignAction>();
	changeFocusOn->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOn->initializeAction("${0} SET 1");
	clickInside->registerAction(changeFocusOn);
	_entity->createComponent<InteractionComponent>()->attachOperation(clickInside, InteractionType::MOUSE_START);
	//--- 2

	//--- 3
	auto clickOutside = std::make_shared<ExpressionOperation>();
	clickOutside->addArgument(_entity);
	clickOutside->initializeOperation("! ( CLICK #{0} )");
	auto changeFocusOff = std::make_shared<AssignAction>();
	changeFocusOff->addArgument(_entity->getComponent<TextComponent>(), "focus");
	changeFocusOff->initializeAction("${0} SET 0");
	clickOutside->registerAction(changeFocusOff);
	_entity->createComponent<InteractionComponent>()->attachOperation(clickOutside, InteractionType::MOUSE_START);
	//--- 3

	return false;
}

bool Label::setPosition(std::tuple<float, float> position) {
	_entity->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(position));
	_entity->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(position));
	return false;
}

bool Label::setSize(std::tuple<float, float> size) {
	_entity->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(size));
	_entity->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(size));
	return false;
}

bool Label::setText(std::string text) {
	_entity->getComponent<TextComponent>()->setText(text);
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

std::shared_ptr<View> LabelFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<Label> label = std::make_shared<Label>(name);
	label->setEntity(_activeScene->createEntity());
	label->getEntity()->createComponent<ObjectComponent>();
	label->getEntity()->createComponent<TextComponent>();
	label->setParent(parent);
	return label;
}

