#include "View.h"

bool View::setEntity(std::shared_ptr<Entity> entity) {
	_entity = entity;
	return false;
}

std::shared_ptr<Entity> View::getEntity() {
	return _entity;
}