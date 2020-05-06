#include "UIActions.h"
#include "GraphicComponents.h"
#include "InteractionOperations.h"
#include "InteractionComponents.h"


PrintComponentsAction::PrintComponentsAction() {
	_actionName = "PrintComponentsAction";
}

bool PrintComponentsAction::doAction() {
	for (auto component : _entity->getComponents()) {
		_list->addItem(component->getName());
	}
	for (auto view : _list->getViews()) {
		auto printItemsOperation = std::make_shared<ExpressionOperation>();
		printItemsOperation->addArgument(view->getEntity());
		//TODO: if view isn't composite need to check entity instead of view
		printItemsOperation->initializeOperation("CLICK #{0}");
		auto printItemsAction = std::make_shared<PrintItemsAction>();
		printItemsAction->setList(_list);
		printItemsAction->setEntity(view->getEntity());
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
