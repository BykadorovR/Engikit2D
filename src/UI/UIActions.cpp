#include "UIActions.h"
#include "GraphicComponents.h"
#include "InteractionOperations.h"
#include "InteractionComponents.h"
#include "InteractionActions.h"
#include "UserInputComponents.h"
#include "CustomComponents.h"

#include <GLFW/glfw3.h>

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
	//create custom component with all names?
	//_views[0]->getEntity()->getComponent<CustomStringArrayComponent>()->addCustomValue(text, "list");

	auto listViews = _list->getViews();
	for (int i = 0; i < listViews.size(); i++) {
		listViews[i]->getEntity()->getComponent<InteractionComponent>()->clearOperations("PrintItems");
		listViews[i]->getEntity()->getComponent<TextComponent>()->setMember("focus", 0);
		auto setEditable = std::make_shared<ExpressionOperation>("PrintItems");
		setEditable->addArgument(listViews[i]->getEntity());
		//TODO: if view isn't composite need to check entity instead of view
		setEditable->initializeOperation("CLICK #{0}");
		auto setEditableAction = std::make_shared<AssignAction>();
		setEditableAction->addArgument(listViews[i]->getEntity()->getComponent<TextComponent>(), "editable");
		setEditableAction->initializeAction("${0} SET 1");
		setEditable->registerAction(setEditableAction);
		auto resetTextAction = std::make_shared<AssignAction>();
		resetTextAction->addArgument(listViews[i]->getEntity()->getComponent<TextComponent>(), "text");
		resetTextAction->initializeAction("${0} SET ");
		setEditable->registerAction(resetTextAction);
		listViews[i]->getEntity()->createComponent<InteractionComponent>()->attachOperation(setEditable, InteractionType::MOUSE_START);

		//TODO: if view isn't composite need to check entity instead of view
		auto changeField = std::make_shared<ExpressionOperation>("PrintItems");
		changeField->addArgument(listViews[i]->getEntity()->getComponent<KeyboardComponent>(), "code");
		changeField->addArgument(nullptr, std::to_string(GLFW_KEY_ENTER));
		changeField->addArgument(listViews[i]->getEntity()->getComponent<TextComponent>(), "focus");
		changeField->initializeOperation("${0} = ${1} AND ${2} = 1");
		auto changeFieldAction = std::make_shared<ApplyItemAction>();
		changeFieldAction->setComponent(component);
		changeFieldAction->setViewIndex(i);
		changeFieldAction->setList(_list);
		changeField->registerAction(changeFieldAction);
		listViews[i]->getEntity()->createComponent<InteractionComponent>()->attachOperation(changeField, InteractionType::KEYBOARD_END);
	}
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


ApplyItemAction::ApplyItemAction() {
	_actionName = "ApplyItemAction";
}

bool ApplyItemAction::doAction() {
	int currentPage = *std::get<0>(_list->getViews()[0]->getEntity()->getComponent<CustomFloatComponent>()->getMemberFloat("page"));
	int componentNameIndex = currentPage + _viewIndex;
	auto componentNames = _component->getItemsNames();
	_component->setMember(componentNames[componentNameIndex], stof(_list->getViews()[_viewIndex]->getEntity()->getComponent<TextComponent>()->getText()));
	return false;
}

bool ApplyItemAction::setList(std::shared_ptr<List> list) {
	_list = list;
	return false;
}

bool ApplyItemAction::setComponent(std::shared_ptr<OperationComponent> component) {
	_component = component;
	return false;
}

bool ApplyItemAction::setViewIndex(int viewIndex) {
	_viewIndex = viewIndex;
	return false;
}
