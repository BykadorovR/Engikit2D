#include "UIActions.h"
#include "GraphicComponents.h"

PrintComponentsAction::PrintComponentsAction() {
	_actionName = "PrintComponentsAction";
}

bool PrintComponentsAction::doAction() {
	for (auto component : _entity->getComponents()) {
		_list->addItem(component->getName());
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

