#pragma once
#include "General.h"
#include "Component.h"
#include <typeinfo>
#include <typeindex>

class Entity {
public:
	int _index = -1;
	/*
	Create component if doesn't exist.
	*/
	template <class ConcreteComponent>
	shared_ptr<ConcreteComponent> createComponent() {
		shared_ptr<ConcreteComponent> component = getComponent<ConcreteComponent>();
		if (component == nullptr) {
			component = make_shared<ConcreteComponent>();
			components.push_back(component);
			return component;
		}
		return component;
	}

	void addComponent(std::shared_ptr<Component> concreteComponent) {
		const std::type_info& targetType = typeid(*concreteComponent.get());
		int componentIndex = -1;
		for (unsigned int i = 0; i < components.size(); i++) {
			const std::type_info& existingType = typeid(*components[i].get());
			if (std::type_index(targetType) == std::type_index(existingType) || components[i].get()->_componentID == concreteComponent.get()->_componentID) {
				componentIndex = i;
			}
		}
		if (componentIndex >= 0) {
			components.erase(components.begin() + componentIndex);
		}

		components.push_back(concreteComponent);
	}


	template <class ConcreteComponent>
	shared_ptr<ConcreteComponent> getComponent() {
		for (auto c : components) {
			shared_ptr<ConcreteComponent> foundComponent = dynamic_pointer_cast<ConcreteComponent>(c);
			if (foundComponent)
				return foundComponent;
		}
		return nullptr;
	}

	template <class ConcreteComponent>
	bool removeComponent() {
		int componentIndex = -1;
		for (unsigned int i = 0; i < components.size(); i++) {
			if (dynamic_pointer_cast<ConcreteComponent>(components[i]))
				componentIndex = i;
		}
		if (componentIndex >= 0) {
			components.erase(components.begin() + componentIndex);
			return true;
		}
		return false;
	}
private:
	std::vector<shared_ptr<Component> > components;
};


//TODO: rewrite as singleton
/*
Class which is used for entities creating/removing ONLY
It just stores all available entities and pull of free indexes for new entities
and assign right index for new entity
*/
class EntityManager {
public:
	shared_ptr<Entity> create(shared_ptr<Entity> _entity = nullptr) {
		shared_ptr<Entity> entity = make_shared<Entity>();
		entities.push_back(entity);
		entity->_index = entities.size() - 1;
		return entities.back();
	}

	bool unregisterEntity(const shared_ptr<Entity>& entity) {
		for (auto local : entities) {
			if (local == entity) {
				entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
				entity->_index = -1;
				return true;
			}
		}
		return false;
	}

	bool registerEntity(shared_ptr<Entity> entity) {
		for (auto local : entities) {
			if (local == entity) {
				return false;
			}
		}
		entities.push_back(entity);
		entity->_index = entities.size() - 1;
		return true;
	}

	vector<shared_ptr<Entity> > getEntities() {
		return entities;
	}

private:
	vector<shared_ptr<Entity> > entities;
};
