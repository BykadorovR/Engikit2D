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

bool View::addView(std::shared_ptr<View> view) {
	_views.push_back(view);
	return false;
}

std::vector<std::shared_ptr<View> > View::getViews() {
	return _views;
}

std::vector<std::shared_ptr<Entity> > View::getEntities() {
	std::vector<std::shared_ptr<Entity> > entities;
	if (_views.size() != 0)
		for (auto view : _views)
			for (auto entity : view->getEntities())
				entities.push_back(entity);
	else
		entities.push_back(_entity);

	return entities;
}

bool View::setParent(std::shared_ptr<View> parent) {
	_parent = parent;
	return false;
}

std::shared_ptr<View> View::getParent() {
	return _parent;
}