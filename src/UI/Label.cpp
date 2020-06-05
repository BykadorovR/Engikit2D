#include "Label.h"
#include "Shader.h"
#include "State.h"
#include "UserInputComponents.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "UIActions.h"
#include "GLFW/glfw3.h"
#include "CustomComponents.h"

Label::Label(std::string name = "Label") {
	_viewName = name;
}

/*
0) if "clicked outside" then "unset focus"
1) if "clicked inside" then "set focus"
2) if "focus && editable && left key" then "decrease cursor position"
3) if "focus && editable && right key" then "increase cursor position"
4) if "focus && editable && backspace" then "remove left from cursor symbol && decrease cursor position"
5) if "focus && editable && enter" then "insert line break && increase cursor position"
6) if "focus && editable && any symbol != "" " then "add symbol && increase cursor position"
*/
bool Label::initialize() {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize({ std::get<0>(resolution) / 2, std::get<1>(resolution) / 2}, {100, 100}, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextComponent>()->initialize(bufferManager);

	_entity->createComponent<MouseComponent>();
	_entity->createComponent<KeyboardComponent>();
	_entity->createComponent<CustomFloatComponent>()->addCustomValue("textStartVertical", 0);

	//--- 0
	{
		auto clickInside = std::make_shared<ExpressionOperation>();
		clickInside->addArgument(_entity, "", "");
		//TODO: Add constants support to operations
		clickInside->initializeOperation("CLICK ${0}");
		auto changeFocusOn = std::make_shared<AssignAction>();
		changeFocusOn->addArgument(_entity, "TextComponent", "focus");
		changeFocusOn->initializeAction("${0} SET 1");
		clickInside->registerAction(changeFocusOn);
		_entity->createComponent<InteractionComponent>()->attachOperation(clickInside, InteractionType::MOUSE_START);
	}

	//--- 1
	{
		auto clickOutside = std::make_shared<ExpressionOperation>();
		clickOutside->addArgument(_entity, "", "");
		clickOutside->initializeOperation("! ( CLICK ${0} )");
		auto changeFocusOff = std::make_shared<AssignAction>();
		changeFocusOff->addArgument(_entity, "TextComponent", "focus");
		changeFocusOff->initializeAction("${0} SET 0");
		clickOutside->registerAction(changeFocusOff);
		_entity->createComponent<InteractionComponent>()->attachOperation(clickOutside, InteractionType::MOUSE_START);
	}

	//--- 2
	{
		auto leftKeyPressed = std::make_shared<ExpressionOperation>();
		leftKeyPressed->addArgument(_entity, "TextComponent", "focus");
		leftKeyPressed->addArgument(_entity, "TextComponent", "editable");
		leftKeyPressed->addArgument(_entity, "KeyboardComponent", "code");
		leftKeyPressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_LEFT));
		leftKeyPressed->addArgument(_entity, "TextComponent", "cursorPosition");
		leftKeyPressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3} AND ${4} > 0");
		auto changeScrollerLeft = std::make_shared<AssignAction>();
		changeScrollerLeft->addArgument(_entity, "TextComponent", "cursorPosition");
		changeScrollerLeft->initializeAction("${0} SET ${0} - 1");
		leftKeyPressed->registerAction(changeScrollerLeft);
		_entity->createComponent<InteractionComponent>()->attachOperation(leftKeyPressed, InteractionType::KEYBOARD_START);
	}

	//--- 3
	{
		auto rightKeyPressed = std::make_shared<ExpressionOperation>();
		rightKeyPressed->addArgument(_entity, "TextComponent", "focus");
		rightKeyPressed->addArgument(_entity, "TextComponent", "editable");
		rightKeyPressed->addArgument(_entity, "KeyboardComponent", "code");
		rightKeyPressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_RIGHT));
		rightKeyPressed->addArgument(_entity, "TextComponent", "text");
		rightKeyPressed->addArgument(_entity, "TextComponent", "cursorPosition");
		rightKeyPressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3} AND ${5} < SIZE ${4}");
		auto changeScrollerRight = std::make_shared<AssignAction>();
		changeScrollerRight->addArgument(_entity, "TextComponent", "cursorPosition");
		changeScrollerRight->initializeAction("${0} SET ${0} + 1");
		rightKeyPressed->registerAction(changeScrollerRight);
		_entity->createComponent<InteractionComponent>()->attachOperation(rightKeyPressed, InteractionType::KEYBOARD_START);
	}

	//--- 4
	{
		auto backspacePressed = std::make_shared<ExpressionOperation>();
		backspacePressed->addArgument(_entity, "TextComponent", "focus");
		backspacePressed->addArgument(_entity, "TextComponent", "editable");
		backspacePressed->addArgument(_entity, "KeyboardComponent", "code");
		backspacePressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_BACKSPACE));
		backspacePressed->addArgument(_entity, "TextComponent", "cursorPosition");
		backspacePressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3} AND ${4} > 0");
		auto removeLastSymbol = std::make_shared<AssignAction>();
		removeLastSymbol->addArgument(_entity, "TextComponent", "text");
		removeLastSymbol->addArgument(_entity, "TextComponent", "cursorPosition");
		removeLastSymbol->initializeAction("${0} REMOVE ${1}");
		backspacePressed->registerAction(removeLastSymbol);
		auto changeScrollerLeft = std::make_shared<AssignAction>();
		changeScrollerLeft->addArgument(_entity, "TextComponent", "cursorPosition");
		changeScrollerLeft->initializeAction("${0} SET ${0} - 1");
		backspacePressed->registerAction(changeScrollerLeft);
		//We should delete at the end
		//TODO: add some priorities, it's really important that remove will be the last
		_entity->createComponent<InteractionComponent>()->attachOperation(backspacePressed, InteractionType::KEYBOARD_END);
	}

	//--- 5
	{
		auto enterPressed = std::make_shared<ExpressionOperation>();
		enterPressed->addArgument(_entity, "TextComponent", "focus");
		enterPressed->addArgument(_entity, "TextComponent", "editable");
		enterPressed->addArgument(_entity, "KeyboardComponent", "code");
		enterPressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_ENTER));
		enterPressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3}");
		auto addBreakLine = std::make_shared<AssignAction>();
		addBreakLine->addArgument(_entity, "TextComponent", "text");
		addBreakLine->addArgument(nullptr, "", "\n");
		addBreakLine->addArgument(_entity, "TextComponent", "cursorPosition");
		addBreakLine->initializeAction("${0} INSERT ${1} ${2}");
		enterPressed->registerAction(addBreakLine);
		auto changeScrollerRight = std::make_shared<AssignAction>();
		changeScrollerRight->addArgument(_entity, "TextComponent", "cursorPosition");
		changeScrollerRight->initializeAction("${0} SET ${0} + 1");
		enterPressed->registerAction(changeScrollerRight);
		_entity->createComponent<InteractionComponent>()->attachOperation(enterPressed, InteractionType::KEYBOARD_START);
	}

	//--- 6
	{
		auto inputSymbol = std::make_shared<ExpressionOperation>();
		inputSymbol->addArgument(_entity, "TextComponent", "focus");
		inputSymbol->addArgument(_entity, "TextComponent", "editable");
		inputSymbol->addArgument(_entity, "KeyboardComponent", "symbol");
		inputSymbol->initializeOperation("${0} = 1 AND ${1} = 1 AND ! ( ${2} =  )");
		auto addSymbol = std::make_shared<AssignAction>();
		addSymbol->addArgument(_entity, "KeyboardComponent", "symbol");
		addSymbol->addArgument(_entity, "TextComponent", "text");
		addSymbol->addArgument(_entity, "TextComponent", "cursorPosition");
		addSymbol->initializeAction("${1} INSERT ${0} ${2}");
		inputSymbol->registerAction(addSymbol);
		auto changeScrollerRight = std::make_shared<AssignAction>();
		changeScrollerRight->addArgument(_entity, "TextComponent", "cursorPosition");
		changeScrollerRight->initializeAction("${0} SET ${0} + 1");
		inputSymbol->registerAction(changeScrollerRight);
		_entity->createComponent<InteractionComponent>()->attachOperation(inputSymbol, InteractionType::KEYBOARD_START);
	}

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
	label->setEntity(_activeScene->createEntity("Label"));
	label->getEntity()->createComponent<ObjectComponent>();
	label->getEntity()->createComponent<TextComponent>();
	label->setParent(parent);
	return label;
}

