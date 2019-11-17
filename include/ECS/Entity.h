#pragma once
#include <memory>

#include "Component.h"

class Entity {
public:
	Entity();
	/*
	Create component if doesn't exist.
	*/
	template <class ConcreteComponent>
	std::shared_ptr<ConcreteComponent> createComponent() {
		shared_ptr<ConcreteComponent> component = getComponent<ConcreteComponent>();
		if (component == nullptr) {
			component = make_shared<ConcreteComponent>();
			_components.push_back(component);
			return component;
		}
		return component;
	}

	template <class ConcreteComponent>
	std::shared_ptr<ConcreteComponent> getComponent() {
		for (auto c : _components) {
			shared_ptr<ConcreteComponent> foundComponent = dynamic_pointer_cast<ConcreteComponent>(c);
			if (foundComponent)
				return foundComponent;
		}
		return nullptr;
	}

	template <class ConcreteComponent>
	bool removeComponent() {
		int componentIndex = -1;
		for (unsigned int i = 0; i < _components.size(); i++) {
			if (dynamic_pointer_cast<ConcreteComponent>(_components[i]))
				componentIndex = i;
		}
		if (componentIndex >= 0) {
			_components.erase(_components.begin() + componentIndex);
			return true;
		}
		return false;
	}

	void addComponent(std::shared_ptr<Component> concreteComponent);
	void clearAllComponents();

	std::vector<std::shared_ptr<Component> > getComponents();

private:
	std::vector<std::shared_ptr<Component> > _components;
	bool _editorMode;
	int _index;
};