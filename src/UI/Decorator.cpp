#include "Decorator.h"
#include "Back.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"


ScrollerDecorator::ScrollerDecorator(std::string name = "ScrollerDecorator") {
	_viewName = name;
}

bool ScrollerDecorator::initialize(std::shared_ptr<Label> label, std::shared_ptr<Shader> shader) {
	auto position = label->getPosition();
	auto size = label->getSize();
	std::tuple<float, float> scrollerSize = { 20, 20 };
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) + std::get<1>(size) - std::get<0>(scrollerSize) };
	std::tuple<float, float> scrollerProgressSize = { 20, 10 };
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(scrollerSize) };

	getScrollerProgress()->initialize(scrollerProgressPosition, scrollerProgressSize, { 0.5, 0.5, 0, 1 }, shader);

	getScrollerUp()->initialize(scrollerUpPosition, scrollerSize, { 1, 0, 0.5, 1 }, shader);
	getScrollerUp()->getEntity()->createComponent<MouseComponent>();
	auto clickInsideUp = std::make_shared<ExpressionOperation>();
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<MouseComponent>(), "leftClickX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<MouseComponent>(), "leftClickY");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	clickInsideUp->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	//TODO: Add constants support to operations
	clickInsideUp->setCondition("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5} AND ${6} > 0");
	clickInsideUp->initializeOperation();

	auto changePositionUp = std::make_shared<AssignAction>();
	changePositionUp->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	changePositionUp->setAction("${0} SET ${0} - 1");
	changePositionUp->initializeAction();
	clickInsideUp->registerAction(changePositionUp);

	auto changePositionProgressUp = std::make_shared<AssignAction>();
	changePositionProgressUp->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	changePositionProgressUp->addArgument(label->getEntity()->getComponent<TextComponent>(), "totalPages");
	changePositionProgressUp->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressUp->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	changePositionProgressUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	changePositionProgressUp->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressUp->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	changePositionProgressUp->setAction("${2} SET ${2} - ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( ${1} - 1 )");
	changePositionProgressUp->initializeAction();
	clickInsideUp->registerAction(changePositionProgressUp);

	getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE);

	getScrollerDown()->initialize(scrollerDownPosition, scrollerSize, { 0, 1, 1, 1 }, shader);
	getScrollerDown()->getEntity()->createComponent<MouseComponent>();
	auto clickInsideDown = std::make_shared<ExpressionOperation>();
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<MouseComponent>(), "leftClickX");
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<MouseComponent>(), "leftClickY");
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionX");
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "sizeX");
	clickInsideDown->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	clickInsideDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	clickInsideDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "totalPages");
	//TODO: Add constants support to operations
	clickInsideDown->setCondition("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5} AND ${6} < ${7} - 1");
	clickInsideDown->initializeOperation();

	auto changePositionDown = std::make_shared<AssignAction>();
	changePositionDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	changePositionDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "totalPages");
	changePositionDown->setAction("${0} SET ${0} + 1");
	changePositionDown->initializeAction();
	clickInsideDown->registerAction(changePositionDown);

	auto changePositionProgressDown = std::make_shared<AssignAction>();
	changePositionProgressDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "page");
	changePositionProgressDown->addArgument(label->getEntity()->getComponent<TextComponent>(), "totalPages");
	changePositionProgressDown->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressDown->addArgument(getScrollerProgress()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	changePositionProgressDown->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressDown->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	changePositionProgressDown->addArgument(getScrollerDown()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	changePositionProgressDown->setAction("${2} SET ${2} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( ${1} - 1 )");
	changePositionProgressDown->initializeAction();
	clickInsideDown->registerAction(changePositionProgressDown);

	getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE);
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

std::shared_ptr<View> ScrollerDecoratorFactory::createView(std::string name) {
	std::shared_ptr<ScrollerDecorator> scrollerDecorator = std::make_shared<ScrollerDecorator>(name);
	scrollerDecorator->setEntity(_activeScene->createEntity());

	scrollerDecorator->setScrollerUp(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerUp")));
	scrollerDecorator->setScrollerDown(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerDown")));
	scrollerDecorator->setScrollerProgress(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerProgress")));
	return scrollerDecorator;
}
