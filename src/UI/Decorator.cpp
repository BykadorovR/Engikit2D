#include "Decorator.h"
#include "Back.h"
#include "Shader.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "CustomComponents.h"

ScrollerDecorator::ScrollerDecorator(std::string name) {
	_viewName = name;
}

/*
List of operation-actions:
1) if "text out of bounds down" (typing) then "scroll text down"
2) if "text out of bounds up" (removing) then "scroll text up"
3) if "click to scroller up" then "scroll text up"
4) if "click to scroller down" then "scroll text down"
5) if "text size > 1" then "change scroller position dynamically depending on current page"
6) if "current page > 1" then "show decorator"
7) if "page == 0 and text size < view height" then "hide decorator"
*/
bool ScrollerDecorator::initialize() {
	std::shared_ptr<Entity> parentEntity = _parent->getEntity();
	//entity == nullptr means that view is composite
	if (parentEntity == nullptr) {
		for (auto view : _parent->getViews()) {
			if (view->getEntity()->getComponent<ObjectComponent>())
				parentEntity = view->getEntity();
		}
	}

	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();
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

	if (_parent->getName() == "Label") {
		//--- 1
		auto textOutOfBoundsDown = std::make_shared<ExpressionOperation>();
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<ObjectComponent>(), "sizeY");
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<TextComponent>(), "lineHeight");
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<TextComponent>(), "spacingCoeff");
		textOutOfBoundsDown->addArgument(parentEntity->getComponent<TextComponent>(), "focus");
		textOutOfBoundsDown->initializeOperation("( ${3} - ${2} ) * ${4} * ${1} > ${0} AND ${5} = 1");
		auto changePageDown = std::make_shared<AssignAction>();
		changePageDown->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		changePageDown->initializeAction("${0} SET ${0} + 1");
		textOutOfBoundsDown->registerAction(changePageDown);
		getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsDown, InteractionType::KEYBOARD_END);
		//--- 1

		//--- 2
		auto textOutOfBoundsUp = std::make_shared<ExpressionOperation>();
		textOutOfBoundsUp->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		textOutOfBoundsUp->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");
		textOutOfBoundsUp->initializeOperation("${1} - ${0} < 1");
		auto changePageUp = std::make_shared<AssignAction>();
		changePageUp->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		changePageUp->initializeAction("${0} SET ${0} - 1");
		textOutOfBoundsUp->registerAction(changePageUp);
		getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(textOutOfBoundsUp, InteractionType::COMMON_START);
		//--- 2
	}

	//--- 3
	auto clickInsideUp = std::make_shared<ExpressionOperation>();
	clickInsideUp->addArgument(getScrollerUp()->getEntity());
	if (_parent->getName() == "Label")
		clickInsideUp->addArgument(parentEntity->getComponent<TextComponent>(), "page");
	else if (_parent->getName() == "List")
		clickInsideUp->addArgument(parentEntity->getComponent<CustomFloatComponent>(), "page");
	clickInsideUp->initializeOperation("CLICK #{0} AND ${0} > 0");
	auto changePositionUp = std::make_shared<AssignAction>();
	if (_parent->getName() == "Label")
		changePositionUp->addArgument(parentEntity->getComponent<TextComponent>(), "page");
	else if (_parent->getName() == "List")
		changePositionUp->addArgument(parentEntity->getComponent<CustomFloatComponent>(), "page");
	changePositionUp->initializeAction("${0} SET ${0} - 1");
	clickInsideUp->registerAction(changePositionUp);
	getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);
	//--- 3

	//--- 4
	auto clickInsideDown = std::make_shared<ExpressionOperation>();
	clickInsideDown->addArgument(getScrollerDown()->getEntity());
	if (_parent->getName() == "Label")
		clickInsideDown->addArgument(parentEntity->getComponent<TextComponent>(), "page");
	else if (_parent->getName() == "List")
		clickInsideDown->addArgument(parentEntity->getComponent<CustomFloatComponent>(), "page");
	if (_parent->getName() == "Label")
		clickInsideDown->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");
	else if (_parent->getName() == "List") {
		clickInsideDown->addArgument(parentEntity->getComponent<CustomStringArrayComponent>(), "list");
		clickInsideDown->addArgument(nullptr, std::to_string(_parent->getViews().size() - 1));
	}
	
	if (_parent->getName() == "Label")
		clickInsideDown->initializeOperation("CLICK #{0} AND ${0} < ${1} - 1");
	else if (_parent->getName() == "List")
		//in case of list we should end scrolling if next scroll will cause displaying less items then supposed (in case of label, 1 item/string can be displayed)
		clickInsideDown->initializeOperation("CLICK #{0} AND ${0} < SIZE ${1} - ${2}");
	auto changePositionDown = std::make_shared<AssignAction>();
	if (_parent->getName() == "Label")
		changePositionDown->addArgument(parentEntity->getComponent<TextComponent>(), "page");
	else if (_parent->getName() == "List")
		changePositionDown->addArgument(parentEntity->getComponent<CustomFloatComponent>(), "page");
	changePositionDown->initializeAction("${0} SET ${0} + 1");
	clickInsideDown->registerAction(changePositionDown);
	getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);
	//--- 4

	//--- 5
	auto positionDecorator = std::make_shared<ExpressionOperation>();
	if (_parent->getName() == "Label")
		positionDecorator->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");
	else if (_parent->getName() == "List") {
		positionDecorator->addArgument(parentEntity->getComponent<CustomStringArrayComponent>(), "list");
		positionDecorator->addArgument(nullptr, std::to_string(_parent->getViews().size() - 1));
	}
	
	//to avoid division by zero
	if (_parent->getName() == "Label")
		positionDecorator->initializeOperation("${0} > 1");
	else if (_parent->getName() == "List")
		positionDecorator->initializeOperation("SIZE ${0} > ${1}");
	auto changePosition = std::make_shared<AssignAction>();
	if (_parent->getName() == "Label")
		changePosition->addArgument(parentEntity->getComponent<TextComponent>(), "page");						    //0
	else if (_parent->getName() == "List")
		changePosition->addArgument(parentEntity->getComponent<CustomFloatComponent>(), "page");					//0
	if (_parent->getName() == "Label")
		changePosition->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");					    //1 
	else if (_parent->getName() == "List")
		changePosition->addArgument(parentEntity->getComponent<CustomStringArrayComponent>(), "list");				//1

	changePosition->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "positionY");  //2
	changePosition->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "sizeY");		//3
	changePosition->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");		//4
	changePosition->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");			//5
	changePosition->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionY");		//6
	if (_parent->getName() == "List") {
		changePosition->addArgument(nullptr, std::to_string(_parent->getViews().size() - 1));		                    //7
	}
	if (_parent->getName() == "Label")
		changePosition->initializeAction("${2} SET ${4} + ${5} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( ${1} - 1 ) * ${0}");
	else if (_parent->getName() == "List")
		changePosition->initializeAction("${2} SET ${4} + ${5} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( SIZE ${1} - ${7} ) * ${0}");
	positionDecorator->registerAction(changePosition);
	getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(positionDecorator, InteractionType::COMMON_START);
	//--- 5
	
	//--- 6
	auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
	if (_parent->getName() == "Label") {
		decoratorAddCheck->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		decoratorAddCheck->initializeOperation("${0} > 0");
	}
	else if (_parent->getName() == "List") {
		decoratorAddCheck->addArgument(parentEntity->getComponent<CustomStringArrayComponent>(), "list");
		decoratorAddCheck->addArgument(nullptr, std::to_string(_parent->getViews().size() - 1));
		decoratorAddCheck->initializeOperation("SIZE ${0} > ${1}");
	}
		
	for (auto view : getViews()) {
		auto registerDecoratorAction = std::make_shared<AssignAction>();
		registerDecoratorAction->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "visible");
		registerDecoratorAction->initializeAction("${0} SET 1");
		decoratorAddCheck->registerAction(registerDecoratorAction);
		if (_parent->getName() == "List")
			view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::COMMON_START);
		else if (_parent->getName() == "Label")
			view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::KEYBOARD_END);
	}
	//--- 6

	//--- 7
	auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
	if (_parent->getName() == "Label") {
		decoratorRemoveCheck->addArgument(parentEntity->getComponent<ObjectComponent>(), "sizeY");
		decoratorRemoveCheck->addArgument(parentEntity->getComponent<TextComponent>(), "page");
		decoratorRemoveCheck->addArgument(parentEntity->getComponent<TextComponent>(), "lineHeight");
		decoratorRemoveCheck->addArgument(parentEntity->getComponent<TextComponent>(), "totalPages");
		decoratorRemoveCheck->initializeOperation("${1} = 0 AND ${3} * ${2} < ${0}");
	}
	else if (_parent->getName() == "List") {
		decoratorRemoveCheck->addArgument(parentEntity->getComponent<CustomStringArrayComponent>(), "list");
		decoratorRemoveCheck->addArgument(nullptr, std::to_string(_parent->getViews().size() - 1));
		decoratorRemoveCheck->initializeOperation("SIZE ${0} < ${1} OR SIZE ${0} = ${1}");
	}
	for (auto view : getViews()) {
		auto unregisterDecoratorAction = std::make_shared<AssignAction>();
		unregisterDecoratorAction->addArgument(view->getEntity()->getComponent<ObjectComponent>(), "visible");
		unregisterDecoratorAction->initializeAction("${0} SET 0");
		decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
		view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
	}
	//--- 7
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

ScrollerDecoratorFactory::ScrollerDecoratorFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> ScrollerDecoratorFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<ScrollerDecorator> scrollerDecorator = std::make_shared<ScrollerDecorator>(name);
	scrollerDecorator->setParent(parent);
	scrollerDecorator->setScrollerUp(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerUp", scrollerDecorator)));
	scrollerDecorator->setScrollerDown(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerDown", scrollerDecorator)));
	scrollerDecorator->setScrollerProgress(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerProgress", scrollerDecorator)));
	
	return scrollerDecorator;
}
