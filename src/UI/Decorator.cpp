#include "Decorator.h"
#include "Back.h"
#include "Shader.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"


ScrollerDecorator::ScrollerDecorator(std::string name, std::shared_ptr<View> parent) {
	_viewName = name;
	_parent = parent;
}

/*
List of operation-actions:
1) if "focus && editable" then "add symbol to text"
2) if "clicked inside" then "set focus"
3) if "clicked outside" then "unset focus"
*/
bool ScrollerDecorator::initialize() {
	auto position = _parent->getEntity()->getComponent<ObjectComponent>()->getPosition();
	auto size = _parent->getEntity()->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> scrollerSize = { 20, 20 };
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) + std::get<1>(size) - std::get<0>(scrollerSize) };
	std::tuple<float, float> scrollerProgressSize = { 20, 10 };
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(scrollerSize) };

	getScrollerProgress()->initialize();
	getScrollerProgress()->setPosition(scrollerProgressPosition);
	getScrollerProgress()->setSize(scrollerProgressSize);
	getScrollerProgress()->setColorMask({0, 0, 0, 0});
	getScrollerProgress()->setColorAddition({ 0.5, 0.5, 0, 1 });
	getScrollerProgress()->getEntity()->createComponent<KeyboardComponent>();

	getScrollerUp()->initialize();
	getScrollerUp()->setPosition(scrollerUpPosition);
	getScrollerUp()->setSize(scrollerSize);
	getScrollerUp()->setColorMask({ 0, 0, 0, 0 });
	getScrollerUp()->setColorAddition({ 1, 0, 0.5, 1 });
	getScrollerUp()->getEntity()->createComponent<MouseComponent>();
	getScrollerUp()->getEntity()->createComponent<KeyboardComponent>();

	getScrollerDown()->initialize();
	getScrollerDown()->setPosition(scrollerDownPosition);
	getScrollerDown()->setSize(scrollerSize);
	getScrollerDown()->setColorMask({ 0, 0, 0, 0 });
	getScrollerDown()->setColorAddition({ 0, 1, 1, 1 });
	getScrollerDown()->getEntity()->createComponent<MouseComponent>();
	getScrollerDown()->getEntity()->createComponent<KeyboardComponent>();
	
	//--- 1
	auto textOutOfBoundsDown = std::make_shared<ExpressionOperation>();
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "lineHeight");
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "spacingCoeff");
	textOutOfBoundsDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "focus");
	textOutOfBoundsDown->initializeOperation("( ${3} - ${2} ) * ${4} * ${1} > ${0} AND ${5} = 1");
	//--- 1

	auto changePageDown = std::make_shared<AssignAction>();
	changePageDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	changePageDown->initializeAction("${0} SET ${0} + 1");
	textOutOfBoundsDown->registerAction(changePageDown);
	getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsDown, InteractionType::KEYBOARD_END);

	auto textOutOfBoundsUp = std::make_shared<ExpressionOperation>();
	textOutOfBoundsUp->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	textOutOfBoundsUp->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	textOutOfBoundsUp->initializeOperation("${1} - ${0} < 1");

	auto changePageUp = std::make_shared<AssignAction>();
	changePageUp->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	changePageUp->initializeAction("${0} SET ${0} - 1");
	textOutOfBoundsUp->registerAction(changePageUp);
	getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsUp, InteractionType::COMMON_START);

	auto clickInsideUp = std::make_shared<ExpressionOperation>();
	clickInsideUp->addArgument(getScrollerUp()->getEntity());
	clickInsideUp->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	//TODO: Add constants support to operations
	clickInsideUp->initializeOperation("CLICK #{0} AND ${0} > 0");

	auto changePositionUp = std::make_shared<AssignAction>();
	changePositionUp->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	changePositionUp->initializeAction("${0} SET ${0} - 1");
	clickInsideUp->registerAction(changePositionUp);
	getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);

	auto clickInsideDown = std::make_shared<ExpressionOperation>();
	clickInsideDown->addArgument(getScrollerDown()->getEntity());
	clickInsideDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	clickInsideDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	//TODO: Add constants support to operations
	clickInsideDown->initializeOperation("CLICK #{0} AND ${0} < ${1} - 1");

	auto changePositionDown = std::make_shared<AssignAction>();
	changePositionDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	changePositionDown->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	changePositionDown->initializeAction("${0} SET ${0} + 1");
	clickInsideDown->registerAction(changePositionDown);
	getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);

	auto positionDecorator = std::make_shared<ExpressionOperation>();
	positionDecorator->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	//to avoid division by zero
	positionDecorator->initializeOperation("${0} > 1");

	auto changePosition = std::make_shared<AssignAction>();
	changePosition->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");							//0
	changePosition->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");					//1 
	changePosition->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "positionY");  //2
	changePosition->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "sizeY");		//3
	changePosition->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");		//4
	changePosition->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");			//5
	changePosition->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionY");		//6
	changePosition->initializeAction("${2} SET ${4} + ${5} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( ${1} - 1 ) * ${0}");
	positionDecorator->registerAction(changePosition);

	getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(positionDecorator, InteractionType::COMMON_START);
	
	auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
	decoratorAddCheck->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	decoratorAddCheck->initializeOperation("${0} > 0");

	for (auto view : getViews()) {
		auto registerDecoratorAction = std::make_shared<AssignAction>();
		registerDecoratorAction->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "visible");
		registerDecoratorAction->initializeAction("${0} SET 1");
		decoratorAddCheck->registerAction(registerDecoratorAction);
		view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::KEYBOARD_END);
	}

	auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
	decoratorRemoveCheck->addArgument(_parent->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	decoratorRemoveCheck->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "page");
	decoratorRemoveCheck->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "lineHeight");
	decoratorRemoveCheck->addArgument(_parent->getEntity()->getComponent<TextComponent>(), "totalPages");
	decoratorRemoveCheck->initializeOperation("${1} = 0 AND ${3} * ${2} < ${0}");

	for (auto view : getViews()) {
		auto unregisterDecoratorAction = std::make_shared<AssignAction>();
		unregisterDecoratorAction->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "visible");
		unregisterDecoratorAction->initializeAction("${0} SET 0");
		decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
		view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
	}

	return false;
}

bool ScrollerDecorator::setScrollerUp(std::shared_ptr<View> scrollerUp) {
	_views.push_back(scrollerUp);
	return false;
}

bool ScrollerDecorator::setScrollerProgress(std::shared_ptr<View> scrollerProgress) {
	_views.push_back(scrollerProgress);
	return false;
}

bool ScrollerDecorator::setScrollerDown(std::shared_ptr<View> scrollerDown) {
	_views.push_back(scrollerDown);
	return false;
}

std::shared_ptr<Back> ScrollerDecorator::getScrollerUp() {
	for (auto view : _views) {
		if (view->getName() == "scrollerUp")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> ScrollerDecorator::getScrollerDown() {
	for (auto view : _views) {
		if (view->getName() == "scrollerDown")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> ScrollerDecorator::getScrollerProgress() {
	for (auto view : _views) {
		if (view->getName() == "scrollerProgress")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::vector<std::shared_ptr<View> > ScrollerDecorator::getViews() {
	return _views;
}

ScrollerDecoratorFactory::ScrollerDecoratorFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> ScrollerDecoratorFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<ScrollerDecorator> scrollerDecorator = std::make_shared<ScrollerDecorator>(name, parent);

	scrollerDecorator->setScrollerUp(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerUp")));
	scrollerDecorator->setScrollerDown(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerDown")));
	scrollerDecorator->setScrollerProgress(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerProgress")));
	return scrollerDecorator;
}
