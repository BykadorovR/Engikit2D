#include "Decorator.h"
#include "Back.h"
#include "List.h"
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

bool ScrollerVerticalDecorator::applyToGrid() {
	std::shared_ptr<Entity> parentEntity = _parent->getViews()[0]->getViews()[0]->getEntity();
	auto size = std::dynamic_pointer_cast<Grid>(_parent->getViews()[0])->getSize();
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	//horizontal size
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	auto lastPosition = _parent->getViews().back()->getViews()[0]->getEntity()->getComponent<ObjectComponent>()->getPosition();
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(lastPosition) + std::get<0>(size), std::get<1>(lastPosition) + std::get<1>(size) - std::get<1>(getScrollerDown()->getSize()) };
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(getScrollerProgress()->getSize()) };
	//TODO: why is it needed at all?
	getScrollerProgress()->setPosition(scrollerProgressPosition);
	getScrollerUp()->setPosition(scrollerUpPosition);
	getScrollerDown()->setPosition(scrollerDownPosition);
	//--- 0
	{
		//TODO: need to make some more smart condition
		auto keepPosition = std::make_shared<ExpressionOperation>();
		keepPosition->initializeOperation("1");
		auto keepPositionUp = std::make_shared<AssignAction>();
		keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionX");
		keepPositionUp->addArgument(getScrollerUp()->getEntity(), "ObjectComponent", "positionY");
		keepPositionUp->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "positionX");
		keepPositionUp->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "positionY");
		keepPositionUp->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "sizeX");
		keepPositionUp->initializeAction("${0} SET ${2} + ${4} AND ${1} SET ${3}");
		keepPosition->registerAction(keepPositionUp);
		auto keepPositionDown = std::make_shared<AssignAction>();
		keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionX");
		keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "positionY");
		keepPositionDown->addArgument(getScrollerDown()->getEntity(), "ObjectComponent", "sizeY");
		keepPositionDown->addArgument(_parent->getViews().back()->getViews().back()->getEntity(), "ObjectComponent", "positionX");
		keepPositionDown->addArgument(_parent->getViews().back()->getViews().back()->getEntity(), "ObjectComponent", "positionY");
		keepPositionDown->addArgument(_parent->getViews().back()->getViews().back()->getEntity(), "ObjectComponent", "sizeX");
		keepPositionDown->addArgument(_parent->getViews().back()->getViews().back()->getEntity(), "ObjectComponent", "sizeY");
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

	//--- 1
	{
		auto clickInsideUp = std::make_shared<ExpressionOperation>();
		clickInsideUp->addArgument(getScrollerUp()->getEntity(), "", "");
		clickInsideUp->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		clickInsideUp->initializeOperation("CLICK ${0} AND ${1} > 0");
		auto changePositionUp = std::make_shared<AssignAction>();
		changePositionUp->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		changePositionUp->initializeAction("${0} SET ${0} - 1");
		clickInsideUp->registerAction(changePositionUp);
		getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);
	}
	//--- 2
	{
		auto clickInsideDown = std::make_shared<ExpressionOperation>();
		clickInsideDown->addArgument(getScrollerDown()->getEntity(), "", "");
		clickInsideDown->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		clickInsideDown->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "CustomStringArrayComponent", "list0");
		clickInsideDown->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
		clickInsideDown->initializeOperation("CLICK ${0} AND ${1} < SIZE ${2} - ${3}");
		auto changePositionDown = std::make_shared<AssignAction>();
		changePositionDown->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		changePositionDown->initializeAction("${0} SET ${0} + 1");
		clickInsideDown->registerAction(changePositionDown);
		getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);
	}
	//--- 3
	{
		auto positionDecorator = std::make_shared<ExpressionOperation>();
		positionDecorator->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "CustomStringArrayComponent", "list0");
		positionDecorator->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
		//to avoid division by zero
		positionDecorator->initializeOperation("SIZE ${0} > ${1}");
		auto changePosition = std::make_shared<AssignAction>();
		changePosition->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");				//0
		changePosition->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "CustomStringArrayComponent", "list0");	//1
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

	//TODO: Transfer visibility check to parent and just copy visibility flag of parent
	//--- 4
	{
		auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
		decoratorAddCheck->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "CustomStringArrayComponent", "list0");
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

	//--- 5
	{
		auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
		decoratorRemoveCheck->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "CustomStringArrayComponent", "list0");
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

	return false;
}

/*
List:
0) if "1" then "attach decorator to view"
1) if "clicked decorator up && list start vertical > 0" then "list start vertical - 1"
2) if "clicked decorator down && list start vertical < list size - number of list entities" then "list start vertical + 1"
3) if "list size > number of list entities" then "change decorator progress position depending on list start vertical" (attached to progress entity)
4) if "list size > number of list entities" then "make decorator visible" (attached to parent entity)
5) if "list size <= number of list entities" then "make decorator invisible"
*/
bool ScrollerVerticalDecorator::applyToList() {
	std::shared_ptr<Entity> parentEntity = _parent->getViews()[0]->getEntity();
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	auto lastPosition = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto lastSize = parentEntity->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(lastPosition) + std::get<0>(lastSize), std::get<1>(lastPosition) + std::get<1>(lastSize) - std::get<1>(getScrollerDown()->getSize()) };
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(getScrollerProgress()->getSize()) };
	getScrollerProgress()->setPosition(scrollerProgressPosition);
	getScrollerUp()->setPosition(scrollerUpPosition);
	getScrollerDown()->setPosition(scrollerDownPosition);
	//--- 0
	{
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

	//--- 1
	{
		auto clickInsideUp = std::make_shared<ExpressionOperation>();
		clickInsideUp->addArgument(getScrollerUp()->getEntity(), "", "");
		clickInsideUp->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		clickInsideUp->initializeOperation("CLICK ${0} AND ${1} > 0");
		auto changePositionUp = std::make_shared<AssignAction>();
		changePositionUp->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		changePositionUp->initializeAction("${0} SET ${0} - 1");
		clickInsideUp->registerAction(changePositionUp);
		getScrollerUp()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideUp, InteractionType::MOUSE_START);
	}
	//--- 2
	{
		auto clickInsideDown = std::make_shared<ExpressionOperation>();
		clickInsideDown->addArgument(getScrollerDown()->getEntity(), "", "");
		clickInsideDown->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		clickInsideDown->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
		clickInsideDown->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
		clickInsideDown->initializeOperation("CLICK ${0} AND ${1} < SIZE ${2} - ${3}");
		auto changePositionDown = std::make_shared<AssignAction>();
		changePositionDown->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");
		changePositionDown->initializeAction("${0} SET ${0} + 1");
		clickInsideDown->registerAction(changePositionDown);
		getScrollerDown()->getEntity()->createComponent<InteractionComponent>()->attachOperation(clickInsideDown, InteractionType::MOUSE_START);
	}
	//--- 3
	{
		auto positionDecorator = std::make_shared<ExpressionOperation>();
		positionDecorator->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
		positionDecorator->addArgument(nullptr, "", std::to_string(_parent->getViews().size()));
		//to avoid division by zero
		positionDecorator->initializeOperation("SIZE ${0} > ${1}");
		auto changePosition = std::make_shared<AssignAction>();
		changePosition->addArgument(parentEntity, "CustomFloatComponent", "listStartVertical");				//0
		changePosition->addArgument(parentEntity, "CustomStringArrayComponent", "list0");					//1
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

	//TODO: Transfer visibility check to parent and just copy visibility flag of parent
	//--- 4
	{
		auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
		decoratorAddCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
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

	//--- 5
	{
		auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
		decoratorRemoveCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
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

	return false;
}

/*
Label:
0) if "1" then "attach decorator to view"
1) if "focus && editable && enter" then "text total pages + 1"
2) if "focus && editable && backspace && text under cursor = '\n'" then "text total pages - 1"
3) if "focus && editable && enter && height of visible text + new line > object height" then "text start vertical + 1"
4) if "clicked decorator up && text start vertical > 0" then "text start vertical - 1"
5) if "clicked decorator down && text start vertical < text total pages" then "text start vertical + 1"
6) if "text total pages > 0" then "change decorator progress position depending on text start vertical"
7) if "text start vertical > 0" then "make decorator visible"
8) if "text start vertical = 0 && text total pages < object height" then "make decorator invisible"
*/
bool ScrollerVerticalDecorator::applyToLabel() {
	std::shared_ptr<Entity> parentEntity = _parent->getEntity();
	//set intial position
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> scrollerUpPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) };
	std::tuple<float, float> scrollerDownPosition = { std::get<0>(position) + std::get<0>(size), std::get<1>(position) + std::get<1>(size) - std::get<1>(getScrollerDown()->getSize()) };
	//if view is composite attach scroller down to last view
	std::tuple<float, float> scrollerProgressPosition = { std::get<0>(scrollerUpPosition), std::get<1>(scrollerUpPosition) + std::get<1>(getScrollerDown()->getSize()) };
	getScrollerProgress()->setPosition(scrollerProgressPosition);
	getScrollerUp()->setPosition(scrollerUpPosition);
	getScrollerDown()->setPosition(scrollerDownPosition);

	//in init we should parse text and find all \n to do initial initialization
	auto text = parentEntity->getComponent<TextComponent>()->getText();
	int totalPages = 0;
	for (int i = 0; i < text.size(); i++) {
		if (text[i] == '\n')
			totalPages++;
	}
	parentEntity->createComponent<CustomFloatComponent>()->addCustomValue("textTotalPages", totalPages);

	//--- 0 
	{
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

	//--- 1
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

	//--- 2
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

	//--- 3
	{
		auto textIsInBounds = std::make_shared<ExpressionOperation>();
		textIsInBounds->addArgument(parentEntity, "TextComponent", "focus");
		textIsInBounds->addArgument(parentEntity, "TextComponent", "editable");
		textIsInBounds->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
		textIsInBounds->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
		textIsInBounds->addArgument(parentEntity, "ObjectComponent", "sizeY");
		textIsInBounds->addArgument(nullptr, "", std::to_string(GlyphsLoader::instance().getGlyphHeight()));
		textIsInBounds->addArgument(parentEntity, "KeyboardComponent", "code");
		textIsInBounds->addArgument(nullptr, "", std::to_string(GLFW_KEY_ENTER));
		//+1 because textTotalPages doesn't take in account first line
		textIsInBounds->initializeOperation("${0} = 1 AND ${1} = 1 AND ${6} = ${7} AND ( ${2} + 1 - ${3} ) * ${5} > ${4}");
		auto increaseStartPage = std::make_shared<AssignAction>();
		increaseStartPage->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
		increaseStartPage->initializeAction("${0} SET ${0} + 1");
		textIsInBounds->registerAction(increaseStartPage);
		parentEntity->createComponent<InteractionComponent>()->attachOperation(textIsInBounds, InteractionType::KEYBOARD_START);
	}

	//--- 4
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

	//--- 5
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

	//--- 6
	{
		auto positionDecorator = std::make_shared<ExpressionOperation>();
		positionDecorator->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages");
		//to avoid division by zero
		positionDecorator->initializeOperation("${0} > 0");
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

	//--- 7
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

	//--- 8
	{
		auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
		decoratorRemoveCheck->addArgument(parentEntity, "ObjectComponent", "sizeY");
		decoratorRemoveCheck->addArgument(parentEntity, "CustomFloatComponent", "textStartVertical");
		decoratorRemoveCheck->addArgument(nullptr, "", std::to_string(GlyphsLoader::instance().getGlyphHeight()));
		decoratorRemoveCheck->addArgument(parentEntity, "CustomFloatComponent", "textTotalPages"); //equal to 0 for first row
		decoratorRemoveCheck->initializeOperation("${1} = 0 AND ( ${3} + 1 ) * ${2} < ${0}");
		for (auto view : getViews()) {
			auto unregisterDecoratorAction = std::make_shared<AssignAction>();
			unregisterDecoratorAction->addArgument(view->getEntity(), "ObjectComponent", "visible");
			unregisterDecoratorAction->initializeAction("${0} SET 0");
			decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
			view->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
		}
	}

	return false;
}

//TODO: make child class for grid/label/list decorators
bool ScrollerVerticalDecorator::initialize() {
	std::tuple<float, float> scrollerSize = { 20, 20 };
	std::tuple<float, float> scrollerProgressSize = { 20, 10 };

	getScrollerProgress()->initialize();
	getScrollerProgress()->setSize(scrollerProgressSize);
	getScrollerProgress()->setColorMask({ 0, 0, 0, 0 });
	getScrollerProgress()->setColorAddition({ 0.5, 0.5, 0, 1 });
	getScrollerProgress()->getEntity()->createComponent<MouseComponent>();
	getScrollerProgress()->getEntity()->createComponent<KeyboardComponent>();

	getScrollerUp()->initialize();
	getScrollerUp()->setSize(scrollerSize);
	getScrollerUp()->setColorMask({ 0, 0, 0, 0 });
	getScrollerUp()->setColorAddition({ 1, 0, 0.5, 1 });
	getScrollerUp()->getEntity()->createComponent<MouseComponent>();
	getScrollerUp()->getEntity()->createComponent<KeyboardComponent>();

	getScrollerDown()->initialize();
	getScrollerDown()->setSize(scrollerSize);
	getScrollerDown()->setColorMask({ 0, 0, 0, 0 });
	getScrollerDown()->setColorAddition({ 0, 1, 1, 1 });
	getScrollerDown()->getEntity()->createComponent<MouseComponent>();
	getScrollerDown()->getEntity()->createComponent<KeyboardComponent>();

	if (std::dynamic_pointer_cast<Label>(_parent)) {
		applyToLabel();
	} else if (std::dynamic_pointer_cast<List>(_parent)) {
		if (std::dynamic_pointer_cast<Grid>(_parent->getViews()[0])) {
			applyToGrid();
		}
		else if (std::dynamic_pointer_cast<Label>(_parent->getViews()[0])) {
			applyToList();
		}
		
	}
	return false;
}

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

bool HeaderDecorator::applyToGrid() {
	std::shared_ptr<Entity> parentEntity = _parent->getViews()[0]->getViews()[0]->getEntity();
	auto size = std::dynamic_pointer_cast<Grid>(_parent->getViews()[0])->getSize();
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	//TODO: add support for multiline grids in header
	std::tuple<float, float> headerSize = { std::get<0>(size), std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) };

	getBack()->setSize(headerSize);
	getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });

	getLabel()->setSize(headerSize);
	getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });
	
	//--- 0
	{
		auto keepSize = std::make_shared<ExpressionOperation>();
		keepSize->initializeOperation("1");
		auto keepSizeBack = std::make_shared<AssignAction>();
		keepSizeBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeBack->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepSizeBack->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "positionX");
		keepSizeBack->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "sizeX");
		keepSizeBack->initializeAction("${0} SET ${2} + ${3} - ${1}");
		keepSize->registerAction(keepSizeBack);
		auto keepSizeLabel = std::make_shared<AssignAction>();
		keepSizeLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeLabel->addArgument(_parent->getViews()[0]->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepSizeLabel->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "positionX");
		keepSizeLabel->addArgument(_parent->getViews()[0]->getViews().back()->getEntity(), "ObjectComponent", "sizeX");
		keepSizeLabel->initializeAction("${0} SET ${2} + ${3} - ${1}");
		keepSize->registerAction(keepSizeLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepSize, InteractionType::COMMON_START);

	}

	//--- 1
	{
		//TODO: need to make some more smart condition, remove COMMON
		auto keepPosition = std::make_shared<ExpressionOperation>();
		keepPosition->initializeOperation("1");
		auto keepPositionBack = std::make_shared<AssignAction>();
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionBack->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionBack);
		auto keepPositionLabel = std::make_shared<AssignAction>();
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionLabel->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepPosition, InteractionType::COMMON_START);
	}

	return false;
}

bool HeaderDecorator::applyToList() {
	std::shared_ptr<Entity> parentEntity = _parent->getViews()[0]->getEntity();
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();
	std::tuple<float, float> headerSize = { std::get<0>(size), std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) };

	getBack()->setSize(headerSize);
	getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });

	getLabel()->setSize(headerSize);
	getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });

	//--- 0
	{
		auto keepSize = std::make_shared<ExpressionOperation>();
		keepSize->initializeOperation("1");
		auto keepSizeBack = std::make_shared<AssignAction>();
		keepSizeBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeBack->addArgument(parentEntity, "ObjectComponent", "sizeX");
		keepSizeBack->addArgument(nullptr, "", std::to_string(std::get<0>(getLabel()->getDim())));
		keepSizeBack->initializeAction("${0} SET ( ${1} / ${2} )");
		keepSize->registerAction(keepSizeBack);
		auto keepSizeLabel = std::make_shared<AssignAction>();
		keepSizeLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeLabel->addArgument(parentEntity, "ObjectComponent", "sizeX");
		keepSizeLabel->addArgument(nullptr, "", std::to_string(std::get<0>(getLabel()->getDim())));
		keepSizeLabel->initializeAction("${0} SET ( ${1} / ${2} )");
		keepSize->registerAction(keepSizeLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepSize, InteractionType::COMMON_START);

	}

	//--- 1
	{
		//TODO: need to make some more smart condition, remove COMMON
		auto keepPosition = std::make_shared<ExpressionOperation>();
		keepPosition->initializeOperation("1");
		auto keepPositionBack = std::make_shared<AssignAction>();
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionBack->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionBack);
		auto keepPositionLabel = std::make_shared<AssignAction>();
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionLabel->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepPosition, InteractionType::COMMON_START);
	}

	//--- 2
	{
		auto decoratorAddCheck = std::make_shared<ExpressionOperation>();
		decoratorAddCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
		decoratorAddCheck->initializeOperation("SIZE ${0} > 0");
		for (auto view : getViews()) {
			auto registerDecoratorAction = std::make_shared<AssignAction>();
			registerDecoratorAction->addArgument(view->getViews()[0]->getEntity(), "ObjectComponent", "visible");
			registerDecoratorAction->initializeAction("${0} SET 1");
			decoratorAddCheck->registerAction(registerDecoratorAction);
		}
		parentEntity->createComponent<InteractionComponent>()->attachOperation(decoratorAddCheck, InteractionType::COMMON_START);
	}

	//--- 3
	{
		auto decoratorRemoveCheck = std::make_shared<ExpressionOperation>();
		decoratorRemoveCheck->addArgument(parentEntity, "CustomStringArrayComponent", "list0");
		decoratorRemoveCheck->initializeOperation("SIZE ${0} = 0");
		for (auto view : getViews()) {
			auto unregisterDecoratorAction = std::make_shared<AssignAction>();
			unregisterDecoratorAction->addArgument(view->getViews()[0]->getEntity(), "ObjectComponent", "visible");
			unregisterDecoratorAction->initializeAction("${0} SET 0");
			decoratorRemoveCheck->registerAction(unregisterDecoratorAction);
			view->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(decoratorRemoveCheck, InteractionType::COMMON_START);
		}
	}

	return false;
}

bool HeaderDecorator::applyToLabel() {
	std::shared_ptr<Entity> parentEntity = _parent->getEntity();
	auto position = parentEntity->getComponent<ObjectComponent>()->getPosition();
	auto size = parentEntity->getComponent<ObjectComponent>()->getSize();


	std::tuple<float, float> headerSize = { std::get<0>(size), std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) };

	getBack()->setSize(headerSize);
	getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });

	getLabel()->setSize(headerSize);
	getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(headerSize) });

	//--- 0
	{
		auto keepSize = std::make_shared<ExpressionOperation>();
		keepSize->initializeOperation("1");
		auto keepSizeBack = std::make_shared<AssignAction>();
		keepSizeBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeBack->addArgument(parentEntity, "ObjectComponent", "sizeX");
		keepSizeBack->addArgument(nullptr, "", std::to_string(std::get<0>(getLabel()->getDim())));
		keepSizeBack->initializeAction("${0} SET ( ${2} / ${4} )");
		keepSize->registerAction(keepSizeBack);
		auto keepSizeLabel = std::make_shared<AssignAction>();
		keepSizeLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeX");
		keepSizeLabel->addArgument(parentEntity, "ObjectComponent", "sizeX");
		keepSizeLabel->addArgument(nullptr, "", std::to_string(std::get<0>(getLabel()->getDim())));
		keepSizeLabel->initializeAction("${0} SET ( ${2} / ${4} )");
		keepSize->registerAction(keepSizeLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepSize, InteractionType::COMMON_START);

	}

	//--- 1
	{
		//TODO: need to make some more smart condition, remove COMMON
		auto keepPosition = std::make_shared<ExpressionOperation>();
		keepPosition->initializeOperation("1");
		auto keepPositionBack = std::make_shared<AssignAction>();
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionBack->addArgument(getBack()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionBack->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionBack->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionBack);
		auto keepPositionLabel = std::make_shared<AssignAction>();
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "positionY");
		keepPositionLabel->addArgument(getLabel()->getViews()[0]->getEntity(), "ObjectComponent", "sizeY");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionX");
		keepPositionLabel->addArgument(parentEntity, "ObjectComponent", "positionY");
		keepPositionLabel->initializeAction("${0} SET ${3} AND ${1} SET ${4} - ${2}");
		keepPosition->registerAction(keepPositionLabel);
		getBack()->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(keepPosition, InteractionType::COMMON_START);
	}

	//--- 2
	{
		//TODO: PERFORMANCE: make up some approach to "link" variables between different entities, so one changed and trigger changes in others
		auto decoratorVisible = std::make_shared<ExpressionOperation>();
		decoratorVisible->initializeOperation("1");
		for (auto view : getViews()) {
			auto decoratorVisibleAction = std::make_shared<AssignAction>();
			decoratorVisibleAction->addArgument(parentEntity, "ObjectComponent", "visible");
			decoratorVisibleAction->addArgument(view->getViews()[0]->getEntity(), "ObjectComponent", "visible");
			decoratorVisibleAction->initializeAction("${1} SET ${0}");
			decoratorVisible->registerAction(decoratorVisibleAction);
		}
		parentEntity->createComponent<InteractionComponent>()->attachOperation(decoratorVisible, InteractionType::KEYBOARD_END);
	}

	return false;
}

HeaderDecorator::HeaderDecorator(std::string name) {
	_viewName = name;
}

bool HeaderDecorator::initialize() {
	getBack()->initialize();
	for (auto& view : getBack()->getViews()) {
		std::dynamic_pointer_cast<Back>(view)->setColorMask({ 0, 0, 0, 0 });
		std::dynamic_pointer_cast<Back>(view)->setColorAddition({ 0.5, 0.5, 0, 1 });
	}

	getLabel()->initialize();
	if (std::dynamic_pointer_cast<Label>(_parent)) {
		applyToLabel();
	}
	else if (std::dynamic_pointer_cast<List>(_parent)) {
		if (std::dynamic_pointer_cast<Grid>(_parent->getViews()[0])) {
			applyToGrid();
		}
		else if (std::dynamic_pointer_cast<Label>(_parent->getViews()[0])) {
			applyToList();
		}
	}

	return false;
}

bool HeaderDecorator::setText(std::vector<std::string> text) {
	auto gridViews = getLabel()->getViews();
	for (int i = 0; i < gridViews.size(); i++) {
		std::dynamic_pointer_cast<Label>(gridViews[i])->setText(text[i]);
	}
	return false;
}

std::shared_ptr<Grid> HeaderDecorator::getBack() {
	for (auto view : _views) {
		if (view->getName() == "Back")
			return std::dynamic_pointer_cast<Grid>(view);
	}
	return nullptr;
}

std::shared_ptr<Grid> HeaderDecorator::getLabel() {
	for (auto view : _views) {
		if (view->getName() == "Label")
			return std::dynamic_pointer_cast<Grid>(view);
	}
	return nullptr;
}

HeaderDecoratorFactory::HeaderDecoratorFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
	_labelFactory = std::make_shared<LabelFactory>(activeScene);
}

std::shared_ptr<View> HeaderDecoratorFactory::createGrid(std::tuple<int, int> dim, std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<HeaderDecorator> headerDecorator = std::make_shared<HeaderDecorator>(name);
	headerDecorator->setParent(parent);
	headerDecorator->addView(_backFactory->createGrid(dim, "Back", headerDecorator));
	headerDecorator->addView(_labelFactory->createGrid(dim, "Label", headerDecorator));

	return headerDecorator;
}

std::shared_ptr<View> HeaderDecoratorFactory::createView(std::string name, std::shared_ptr<View> parent) {
	return createGrid({ 1, 1 }, name, parent);
}