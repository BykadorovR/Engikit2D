#pragma once
#include "General.h"
#include "Component.h"

class Entity {
public:
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
		return nullptr;
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
	vector<shared_ptr<Component> > components;
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
		return entities.back();
	}

	bool unregister(const shared_ptr<Entity>& entity) {
		for (auto local : entities) {
			if (local == entity) {
				entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
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
		return true;
	}

	vector<shared_ptr<Entity> > getEntities() {
		return entities;
	}

private:
	vector<shared_ptr<Entity> > entities;
};