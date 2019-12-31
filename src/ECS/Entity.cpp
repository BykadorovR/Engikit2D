#include "Entity.h"

Entity::Entity() {
	_index = -1;
	_editorMode = true;
}

int Entity::getIndex() {
	return _index;
}

void Entity::setIndex(int index) {
	_index = index;
}

void Entity::addComponent(std::shared_ptr<Component> concreteComponent) {
	int componentIndex = -1;
	for (unsigned int i = 0; i < _components.size(); i++) {
		const std::type_info& existingType = typeid(*_components[i].get());
		if (_components[i].get()->_componentName == concreteComponent.get()->_componentName) {
			componentIndex = i;
		}
	}
	if (componentIndex >= 0) {
		_components.erase(_components.begin() + componentIndex);
	}

	_components.push_back(concreteComponent);
}

void Entity::clearAllComponents() {
	_components.clear();
}

std::vector<std::shared_ptr<Component> > Entity::getComponents() {
	return _components;
}