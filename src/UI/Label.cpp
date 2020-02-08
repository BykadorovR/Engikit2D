#include "Label.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"

Label::Label(std::string name = "Label") {
	_viewName = name;
}

bool Label::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::wstring text, std::shared_ptr<GlyphsLoader> glyphs, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, shader);
	_entity->getComponent<TextComponent>()->initialize(text, glyphs, bufferManager);
	std::tuple<float, float> scrollerSize = { 20, 20 };
	std::tuple<float, float> scrollerUpPosition = {std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) + std::get<1>(size) -  std::get<0>(scrollerSize)};
	//TODO: how to pass texture here?

	getScrollerUp()->initialize(scrollerUpPosition, scrollerSize, {1, 0, 1, 1}, shader);
	getScrollerUp()->getEntity()->createComponent<MouseComponent>();
	auto clickInsideUp = std::make_shared<ExpressionOperation>();
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<MouseComponent>(), "leftClickX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<MouseComponent>(), "leftClickY");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "positionY");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeX");
	clickInsideUp->addArgument(getScrollerUp()->getEntity()->getComponent<ObjectComponent>(), "sizeY");
	clickInsideUp->addArgument(_entity->getComponent<TextComponent>(), "page");
	//TODO: Add constants support to operations
	clickInsideUp->setCondition("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5} AND ${6} > 0");
	clickInsideUp->initializeOperation();

	auto changePositionUp = std::make_shared<AssignAction>();
	changePositionUp->addArgument(_entity->getComponent<TextComponent>(), "page");
	changePositionUp->addArgument(_entity->getComponent<TextComponent>(), "totalPages");
	changePositionUp->setAction("${0} SET ${0} - 1");
	changePositionUp->initializeAction();
	clickInsideUp->registerAction(changePositionUp);
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
	clickInsideDown->addArgument(_entity->getComponent<TextComponent>(), "page");
	clickInsideDown->addArgument(_entity->getComponent<TextComponent>(), "totalPages");
	//TODO: Add constants support to operations
	clickInsideDown->setCondition("${0} > ${2} AND ${0} < ${2} + ${4} AND ${1} > ${3} AND ${1} < ${3} + ${5} AND ${6} < ${7} - 1");
	clickInsideDown->initializeOperation();

	auto changePositionDown = std::make_shared<AssignAction>();
	changePositionDown->addArgument(_entity->getComponent<TextComponent>(), "page");
	changePositionDown->addArgument(_entity->getComponent<TextComponent>(), "totalPages");
	changePositionDown->setAction("${0} SET ${0} + 1");
	changePositionDown->initializeAction();
	clickInsideDown->registerAction(changePositionDown);
	getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE);
	return false;
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

bool Label::setScrollerUp(std::shared_ptr<View> scrollerUp) {
	_views.push_back(scrollerUp);
	return false;
}

bool Label::setScrollerDown(std::shared_ptr<View> scrollerDown) {
	_views.push_back(scrollerDown);
	return false;
}

std::shared_ptr<Back> Label::getScrollerUp() {
	for (auto view : _views) {
		if (view->getName() == "scrollerUp")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> Label::getScrollerDown() {
	for (auto view : _views) {
		if (view->getName() == "scrollerDown")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
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
	label->setScrollerUp(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerUp")));
	label->setScrollerDown(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerDown")));
	return label;
}

