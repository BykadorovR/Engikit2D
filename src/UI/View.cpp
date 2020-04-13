#include "View.h"

View::View() {
	_viewName = "View";
}

std::string View::getName() {
	return _viewName;
}

bool View::setEntity(std::shared_ptr<Entity> entity) {
	_entity = entity;
	return false;
}

std::shared_ptr<Entity> View::getEntity() {
	return _entity;
}