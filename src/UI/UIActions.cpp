#include "UIActions.h"
#include "GraphicComponents.h"
#include "InteractionOperations.h"
#include "InteractionComponents.h"
#include "InteractionActions.h"
#include "UserInputComponents.h"

PrintComponentsAction::PrintComponentsAction() {
	_actionName = "PrintComponentsAction";
}

bool PrintComponentsAction::doAction() {
	_list->clear();

	for (auto component : _entity->getComponents()) {
		_list->addItem(component->getName());
	}
		
	for (auto view : _list->getViews()) {
		view->getEntity()->getComponent<InteractionComponent>()->clearOperations("PrintItems");
		auto printItemsOperation = std::make_shared<ExpressionOperation>("PrintItems");
		printItemsOperation->addArgument(view->getEntity());
		//TODO: if view isn't composite need to check entity instead of view
		printItemsOperation->initializeOperation("CLICK #{0}");
		auto printItemsAction = std::make_shared<PrintItemsAction>();
		printItemsAction->setList(_list);
		printItemsAction->setEntity(_entity);
		printItemsAction->setComponent(view->getEntity()->getComponent<TextComponent>());
		printItemsOperation->registerAction(printItemsAction);
		view->getEntity()->createComponent<InteractionComponent>()->attachOperation(printItemsOperation, InteractionType::MOUSE_START);
	}
	return false;
}

bool PrintComponentsAction::setList(std::shared_ptr<List> list) {
	_list = list;
	return false;
}

bool PrintComponentsAction::setEntity(std::shared_ptr<Entity> entity) {
	_entity = entity;
	return false;
}

ClearComponentsAction::ClearComponentsAction() {
	_actionName = "ClearComponentsAction";
}
bool ClearComponentsAction::doAction() {
	_list->clear();
	return false;
}
bool ClearComponentsAction::setList(std::shared_ptr<List> list) {
	_list = list;
	return false;
}
bool ClearComponentsAction::setEntity(std::shared_ptr<Entity> entity) {
	_entity = entity;
	return false;
}

PrintItemsAction::PrintItemsAction() {
	_actionName = "PrintItemsAction";
}

bool PrintItemsAction::doAction() {
	//TODO: remove op/act from list items
	auto component = std::dynamic_pointer_cast<OperationComponent>(_entity->getComponent(_component->getText()));
	_list->clear();

	for (auto item : component->getItemsNames()) {
		_list->addItem(item);
	}

	for (auto view : _list->getViews()) {
		view->getEntity()->getComponent<InteractionComponent>()->clearOperations("PrintItems");
		view->getEntity()->getComponent<TextComponent>()->setMember("focus", 0);
		auto setEditable = std::make_shared<ExpressionOperation>("PrintItems");
		setEditable->addArgument(view->getEntity());
		//TODO: if view isn't composite need to check entity instead of view
		setEditable->initializeOperation("CLICK #{0}");
		auto setEditableAction = std::make_shared<AssignAction>();
		setEditableAction->addArgument(view->getEntity()->getComponent<TextComponent>(), "editable");
		setEditableAction->initializeAction("${0} SET 1");
		setEditable->registerAction(setEditableAction);
		auto resetTextAction = std::make_shared<AssignAction>();
		resetTextAction->addArgument(view->getEntity()->getComponent<TextComponent>(), "text");
		resetTextAction->initializeAction("${0} SET ");
		setEditable->registerAction(resetTextAction);
		_list->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(setEditable, InteractionType::MOUSE_START);

	}
	//TODO: if view isn't composite need to check entity instead of view
	auto changeField = std::make_shared<ExpressionOperation>("PrintItems");
	changeField->addArgument(_list->getViews()[0]->getEntity()->getComponent<KeyboardComponent>(), "symbol");
	changeField->initializeOperation("${0} = 4");
	auto changeFieldAction = std::make_shared<AssignAction>();
	//need to find field from component which is assigned for this view
	//need to understand index of string in list array
	changeFieldAction->addArgument(component, "positionX");
	changeFieldAction->addArgument(_list->getViews()[0]->getEntity()->getComponent<TextComponent>(), "text");
	changeFieldAction->addArgument(_list->getViews()[0]->getEntity()->getComponent<KeyboardComponent>(), "symbol");
	changeFieldAction->initializeAction("${0} SET ${1} AND ${2} SET ");
	changeField->registerAction(changeFieldAction);
	_list->getViews()[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(changeField, InteractionType::COMMON_END);
	return false;
}

bool PrintItemsAction::setList(std::shared_ptr<List> list) {
	_list = list;
	return false;
}

bool PrintItemsAction::setComponent(std::shared_ptr<TextComponent> component) {
	_component = component;
	return false;
}

bool PrintItemsAction::setEntity(std::shared_ptr<Entity> entity) {
	_entity = entity;
	return false;
}
