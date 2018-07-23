#pragma once
#include "General.h"
#include "Component.h"

typedef unsigned int EntityID;

class Entity {
public:
	template <class ConcreteComponent>
	void addComponent(shared_ptr<ConcreteComponent> component) {
		components.push_back(component);
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
	void removeComponent() {
		int componentIndex = -1;
		for (int i = 0; i < components.size(); i++) {
			if (dynamic_pointer_cast<ConcreteComponent>(components[i]))
				componentIndex = i;
		}
		if (componentIndex > 0)
			components.erase(components.begin() + componentIndex);
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
	shared_ptr<Entity> create() {
		shared_ptr<Entity> entity = make_shared<Entity>();
		entities.push_back(entity);
		return entities.back();
	}

	void remove(shared_ptr<Entity> entity) {
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}

	vector<shared_ptr<Entity> > getEntities() {
		return entities;
	}

private:
	vector<shared_ptr<Entity> > entities;
};
