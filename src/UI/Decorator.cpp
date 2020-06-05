#include "Decorator.h"
#include "Back.h"
#include "Shader.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "CustomComponents.h"
#include "GLFW/glfw3.h"

ScrollerVerticalDecorator::ScrollerVerticalDecorator(std::string name) {
	_viewName = name;
}

bool ScrollerVerticalDecorator::initialize() {
	//TODO: we should add custom component to entity?
	std::shared_ptr<Entity> parentEntity = _parent->getEntity();
	//entity == nullptr means that view is composite
	if (parentEntity == nullptr) {
		for (auto view : _parent->getViews()) {
			//attach all components to parent entity or first child view
			if (view->getEntity()->getComponent<ObjectComponent>()) {
				parentEntity = view->getEntity();
				break;
			}
		}
	}

	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> scrollerSize = { 20, 20 };
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) + std::get<1>(size) - std::get<0>(scrollerSize) };
	//if view is composite attach scroller down to last view
	if (_parent->getEntity() == nullptr) {
		auto lastPosition = _parent->getViews().back()->getEntity()->getComponent<ObjectComponent>()->getPosition();
		auto lastSize = _parent->getViews().back()->getEntity()->getComponent<ObjectComponent>()->getSize();
		scrollerDownPosition = { std::get<0>(lastPosition) + std::get<0>(lastSize), std::get<1>(lastPosition) + std::get<1>(lastSize) - std::get<0>(scrollerSize) };
	}
	std::tuple<float, float> scrollerProgressSize = { 20, 10 };
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(scrollerSize) };

	getScrollerProgress()->initialize();
	getScrollerProgress()->setPosition(scrollerProgressPosition);
	getScrollerProgress()->setSize(scrollerProgressSize);
	getScrollerProgress()->setColorMask({ 0, 0, 0, 0 });
	getScrollerProgress()->setColorAddition({ 0.5, 0.5, 0, 1 });
	getScrollerProgress()->getEntity()->createComponent<MouseComponent>();
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
		//in init we should parse text and find all \n to do initial initialization
		auto text = parentEntity->getComponent<TextComponent>()->getText();
		int totalPages = 0;
		for (int i = 0; i < text.size(); i++) {
			if (text[i] == '\n')
				totalPages++;
		}
		parentEntity->createComponent<CustomFloatComponent>()->addCustomValue("textTotalPages", totalPages);

		{
			//--- 0 
			//TODO: need to make some more smart condition, remove COMMON
			auto keepPosition = std::make_shared<ExpressionOperation>();
			keepPosition->initializeOperation("1");
			auto keepPositionUp = std::make_shared<AssignAction>();
			keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionX");
			keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "positionX");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "positionY");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "sizeX");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "sizeY");
			keepPositionUp->initializeAction("${0} SET ${2} + ${4} AND ${1} SET ${3}");
			keepPosition->registerAction(keepPositionUp);
			auto keepPositionDown = std::make_shared<AssignAction>();
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionX");
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionY");
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "sizeY");
			keepPositionDown->addArgument(parentEntity, "ObjectComponent", "positionX");
			keepPositionDown->addArgument(parentEntity, "ObjectComponent", "positionY");
			keepPositionDown->addArgument(parentEntity, "ObjectComponent", "sizeX");
			keepPositionDown->addArgument(parentEntity, "ObjectComponent", "sizeY");
			keepPositionDown->initializeAction("${0} SET ${3} + ${5} AND ${1} SET ${4} + ${6} - ${2}");
			keepPosition->registerAction(keepPositionDown);
			auto keepPositionScroller = std::make_shared<AssignAction>();
			keepPositionScroller->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "positionX");
			keepPositionScroller->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionX");
			keepPositionScroller->initializeAction("${0} SET ${1}");
			keepPosition->registerAction(keepPositionScroller);
			getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepPosition, InteractionType::COMMON_START);

		}


		//if pressed enter = totalPages +1
		//if pressed backspace and symbol under cursor is \n = totalPages -1
		//if totalPages - startPage > object can have = startPage +1
		//--- 0
		{
			auto enterPressed = std::make_shared<ExpressionOperation>();
			enterPressed->addArgument(parentEntity, "TextComponent", "focus");
			enterPressed->addArgument(parentEntity, "TextComponent", "editable");
			enterPressed->addArgument(parentEntity, "KeyboardComponent", "code");
			enterPressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_ENTER));
			enterPressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3}");
			auto increaseTotalPages = std::make_shared<AssignAction>();
			increaseTotalPages->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			increaseTotalPages->initializeAction("${0} SET ${0} + 1");
			enterPressed->registerAction(increaseTotalPages);
			parentEntity->createComponent<InteractionComponent>()->attachOperation(enterPressed, InteractionType::KEYBOARD_START);
		}
		//--- 1
		{
			auto backspacePressed = std::make_shared<ExpressionOperation>();
			backspacePressed->addArgument(parentEntity, "TextComponent", "focus");
			backspacePressed->addArgument(parentEntity, "TextComponent", "editable");
			backspacePressed->addArgument(parentEntity, "KeyboardComponent", "code");
			backspacePressed->addArgument(nullptr, "", std::to_string(GLFW_KEY_BACKSPACE));
			backspacePressed->addArgument(parentEntity, "TextComponent", "text");
			backspacePressed->addArgument(nullptr, "", "\n");
			backspacePressed->addArgument(parentEntity, "TextComponent", "cursorPosition");
			backspacePressed->initializeOperation("${0} = 1 AND ${1} = 1 AND ${2} = ${3} AND ${4} AT ( ${6} - 1 ) = ${5}");
			auto decreaseTotalPages = std::make_shared<AssignAction>();
			decreaseTotalPages->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			decreaseTotalPages->initializeAction("${0} SET ${0} - 1");
			backspacePressed->registerAction(decreaseTotalPages);
			parentEntity->createComponent<InteractionComponent>()->attachOperation(backspacePressed, InteractionType::KEYBOARD_START);
		}

		//--- 2
		{
			auto textIsInBounds = std::make_shared<ExpressionOperation>();
			textIsInBounds->addArgument(parentEntity, "TextComponent", "focus");
			textIsInBounds->addArgument(parentEntity, "TextComponent", "editable");
			textIsInBounds->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			textIsInBounds->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			textIsInBounds->addArgument(parentEntity, "ObjectComponent", "sizeY");
			textIsInBounds->addArgument(nullptr, "", std::to_string(GlyphsLoader::instance().getGlyphHeight()));
			textIsInBounds->initializeOperation("${0} = 1 AND ${1} = 1 AND ( ${2} - ${3} ) * ${5} > ${4}");
			auto increaseStartPage = std::make_shared<AssignAction>();
			increaseStartPage->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			increaseStartPage->initializeAction("${0} SET ${0} + 1");
			textIsInBounds->registerAction(increaseStartPage);
			parentEntity->createComponent<InteractionComponent>()->attachOperation(textIsInBounds, InteractionType::KEYBOARD_START);
		}

		//--- 3
		{
			auto clickInsideUp = std::make_shared<ExpressionOperation>();
			clickInsideUp->addArgument(getScrollerUp()->getEntity(), "", "");
			clickInsideUp->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			clickInsideUp->initializeOperation("CLICK ${0} AND ${1} > 0");
			auto changePositionUp = std::make_shared<AssignAction>();
			changePositionUp->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			changePositionUp->initializeAction("${0} SET ${0} - 1");
			clickInsideUp->registerAction(changePositionUp);
			getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);
		}

		//--- 4
		{
			auto clickInsideDown = std::make_shared<ExpressionOperation>();
			clickInsideDown->addArgument(getScrollerDown()->getEntity(), "", "");
			clickInsideDown->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			clickInsideDown->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			clickInsideDown->initializeOperation("CLICK ${0} AND ${1} < ${2}");
			auto changePositionDown = std::make_shared<AssignAction>();
			changePositionDown->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			changePositionDown->initializeAction("${0} SET ${0} + 1");
			clickInsideDown->registerAction(changePositionDown);
			getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);
		}

		//--- 5
		{
			auto positionDecorator = std::make_shared<ExpressionOperation>();
			positionDecorator->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			positionDecorator->initializeOperation("${0} > 1");
			auto changePosition = std::make_shared<AssignAction>();
			changePosition->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");				//0
			changePosition->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");				//1 
			changePosition->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "positionY");	//2
			changePosition->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "sizeY");		//3
			changePosition->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");			//4
			changePosition->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "sizeY");				//5
			changePosition->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionY");		//6
			changePosition->initializeAction("${2} SET ${4} + ${5} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( ${1} ) * ${0}");
			positionDecorator->registerAction(changePosition);
			getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(positionDecorator, InteractionType::COMMON_START);
		}

		//--- 6
		{
			auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
			decoratorAddCheck->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			decoratorAddCheck->initializeOperation("${0} > 0");
			for (auto view : getViews()) {
				auto registerDecoratorAction = std::make_shared<AssignAction>();
				registerDecoratorAction->addArgument(view->getEntity(), "ObjectComponent", "visible");
				registerDecoratorAction->initializeAction("${0} SET 1");
				decoratorAddCheck->registerAction(registerDecoratorAction);
			}
			parentEntity->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::KEYBOARD_END);
		}

		//--- 7
		{
			auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
			decoratorRemoveCheck->addArgument(parentEntity, "ObjectComponent", "sizeY");
			decoratorRemoveCheck->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
			decoratorRemoveCheck->addArgument(nullptr, "", std::to_string(GlyphsLoader::instance().getGlyphHeight()));
			decoratorRemoveCheck->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
			decoratorRemoveCheck->initializeOperation("${1} = 0 AND ${3} * ${2} < ${0}");
			for (auto view : getViews()) {
				auto unregisterDecoratorAction = std::make_shared<AssignAction>();
				unregisterDecoratorAction->addArgument(view->getEntity(), "ObjectComponent", "visible");
				unregisterDecoratorAction->initializeAction("${0} SET 0");
				decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
				view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
			}
		}

	} else if (_parent->getName() == "List") {
		{
			//--- 0 
			//TODO: need to make some more smart condition
			auto keepPosition = std::make_shared<ExpressionOperation>();
			keepPosition->initializeOperation("1");
			auto keepPositionUp = std::make_shared<AssignAction>();
			keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionX");
			keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "positionX");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "positionY");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "sizeX");
			keepPositionUp->addArgument(parentEntity, "ObjectComponent", "sizeY");
			keepPositionUp->initializeAction("${0} SET ${2} + ${4} AND ${1} SET ${3}");
			keepPosition->registerAction(keepPositionUp);
			auto keepPositionDown = std::make_shared<AssignAction>();
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionX");
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionY");
			keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "sizeY");
			keepPositionDown->addArgument(_parent->getViews().back()->getEntity(), "ObjectComponent", "positionX");
			keepPositionDown->addArgument(_parent->getViews().back()->getEntity(), "ObjectComponent", "positionY");
			keepPositionDown->addArgument(_parent->getViews().back()->getEntity(), "ObjectComponent", "sizeX");
			keepPositionDown->addArgument(_parent->getViews().back()->getEntity(), "ObjectComponent", "sizeY");
			keepPositionDown->initializeAction("${0} SET ${3} + ${5} AND ${1} SET ${4} + ${6} - ${2}");
			keepPosition->registerAction(keepPositionDown);
			auto keepPositionScroller = std::make_shared<AssignAction>();
			keepPositionScroller->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "positionX");
			keepPositionScroller->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "positionY");
			keepPositionScroller->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionX");
			keepPositionScroller->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");
			keepPositionScroller->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "sizeY");
			keepPositionScroller->initializeAction("${0} SET ${2} AND ${1} SET ${3} + ${4}");
			keepPosition->registerAction(keepPositionScroller);
			getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepPosition, InteractionType::COMMON_START);
		}

		//--- 3
		{
			auto clickInsideUp = std::make_shared<ExpressionOperation>();
			clickInsideUp->addArgument(getScrollerUp()->getEntity(), "", "");
			clickInsideUp->addArgument(parentEntity, "CustomFloatComponent", "listStartPage");
			clickInsideUp->initializeOperation("CLICK ${0} AND ${1} > 0");
			auto changePositionUp = std::make_shared<AssignAction>();
			changePositionUp->addArgument(parentEntity, "CustomFloatComponent", "listStartPage");
			changePositionUp->initializeAction("${0} SET ${0} - 1");
			clickInsideUp->registerAction(changePositionUp);
			getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);
		}
		//--- 4
		{
			auto clickInsideDown = std::make_shared<ExpressionOperation>();
			clickInsideDown->addArgument(getScrollerDown()->getEntity(), "", "");
			clickInsideDown->addArgument(parentEntity, "CustomFloatComponent", "listStartPage");
			clickInsideDown->addArgument(parentEntity, "CustomStringArrayComponent", "list");
			clickInsideDown->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
			clickInsideDown->initializeOperation("CLICK ${0} AND ${1} < SIZE ${2} - ${3}");
			auto changePositionDown = std::make_shared<AssignAction>();
			changePositionDown->addArgument(parentEntity, "CustomFloatComponent", "listStartPage");
			changePositionDown->initializeAction("${0} SET ${0} + 1");
			clickInsideDown->registerAction(changePositionDown);
			getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);
		}
		//--- 5
		{
			auto positionDecorator = std::make_shared<ExpressionOperation>();
			positionDecorator->addArgument(parentEntity, "CustomStringArrayComponent", "list");
			positionDecorator->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
			//to avoid division by zero
			positionDecorator->initializeOperation("SIZE ${0} > ${1}");
			auto changePosition = std::make_shared<AssignAction>();
			changePosition->addArgument(parentEntity, "CustomFloatComponent", "listStartPage");					//0
			changePosition->addArgument(parentEntity, "CustomStringArrayComponent", "list");					//1
			changePosition->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "positionY");	//2
			changePosition->addArgument(getScrollerProgress()->getEntity(), "ObjectComponent", "sizeY");		//3
			changePosition->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");			//4
			changePosition->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "sizeY");				//5
			changePosition->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionY");		//6
			changePosition->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));				//7
			changePosition->initializeAction("${2} SET ${4} + ${5} + ( ${6} - ${3} - ( ${4} + ${5} ) ) / ( SIZE ${1} - ${7} ) * ${0}");
			positionDecorator->registerAction(changePosition);
			getScrollerProgress()->getEntity()->createComponent<InteractionComponent>()->attachOperation(positionDecorator, InteractionType::COMMON_START);
		}

		//--- 6
		{
			auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
			decoratorAddCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list");
			decoratorAddCheck->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
			decoratorAddCheck->initializeOperation("SIZE ${0} > ${1}");
			for (auto view : getViews()) {
				auto registerDecoratorAction = std::make_shared<AssignAction>();
				registerDecoratorAction->addArgument(view->getEntity(), "ObjectComponent", "visible");
				registerDecoratorAction->initializeAction("${0} SET 1");
				decoratorAddCheck->registerAction(registerDecoratorAction);
			}
			parentEntity->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::COMMON_START);
		}
		//--- 7
		{
			auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
			decoratorRemoveCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list");
			decoratorRemoveCheck->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
			decoratorRemoveCheck->initializeOperation("SIZE ${0} < ${1} OR SIZE ${0} = ${1}");
			for (auto view : getViews()) {
				auto unregisterDecoratorAction = std::make_shared<AssignAction>();
				unregisterDecoratorAction->addArgument(view->getEntity(), "ObjectComponent", "visible");
				unregisterDecoratorAction->initializeAction("${0} SET 0");
				decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
				view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
			}
		}
	}
	return false;
}

/*
List of operation-actions:
0) keep decorator's position to parent entity
1) if "text out of bounds down" (typing) then "scroll text down"
2) if "text out of bounds up" (removing) then "scroll text up"
3) if "click to scroller up" then "scroll text up"
4) if "click to scroller down" then "scroll text down"
5) if "text size > 1" then "change scroller position dynamically depending on current page"
6) if "current page > 1" then "show decorator"
7) if "page == 0 and text size < view height" then "hide decorator"
*/
bool ScrollerVerticalDecorator::setScrollerUp(std::shared_ptr<View> scrollerUp) {
	_views.push_back(scrollerUp);
	return false;
}

bool ScrollerVerticalDecorator::setScrollerProgress(std::shared_ptr<View> scrollerProgress) {
	_views.push_back(scrollerProgress);
	return false;
}

bool ScrollerVerticalDecorator::setScrollerDown(std::shared_ptr<View> scrollerDown) {
	_views.push_back(scrollerDown);
	return false;
}

std::shared_ptr<Back> ScrollerVerticalDecorator::getScrollerUp() {
	for (auto view : _views) {
		if (view->getName() == "scrollerUp")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> ScrollerVerticalDecorator::getScrollerDown() {
	for (auto view : _views) {
		if (view->getName() == "scrollerDown")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> ScrollerVerticalDecorator::getScrollerProgress() {
	for (auto view : _views) {
		if (view->getName() == "scrollerProgress")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

ScrollerVerticalDecoratorFactory::ScrollerVerticalDecoratorFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> ScrollerVerticalDecoratorFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<ScrollerVerticalDecorator> scrollerDecorator = std::make_shared<ScrollerVerticalDecorator>(name);
	scrollerDecorator->setParent(parent);
	scrollerDecorator->setScrollerUp(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerUp", scrollerDecorator)));
	scrollerDecorator->setScrollerDown(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerDown", scrollerDecorator)));
	scrollerDecorator->setScrollerProgress(std::dynamic_pointer_cast<Back>(_backFactory->createView("scrollerProgress", scrollerDecorator)));
	
	return scrollerDecorator;
}