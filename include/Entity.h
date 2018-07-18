#pragma once
#include "General.h"
#include "Component.h"

typedef unsigned int EntityID;

class Entity {
public:
	template <class ConcreteComponent>
	void addComponent(ConcreteComponent* component) {
		components.push_back(component);
	}

	template <class ConcreteComponent>
	ConcreteComponent* getComponent() {
		//TODO: add code with dynamic cast from components (find right component by class)
		for (auto c : components) {
			ConcreteComponent* foundComponent = dynamic_cast<ConcreteComponent*>(c);
			if (foundComponent)
				return foundComponent;
		}
		return nullptr;
	}

	template <class ConcreteComponent>
	void removeComponent() {
		int componentIndex = -1;
		for (int i = 0; i < components.size(); i++) {
			if (dynamic_cast<ConcreteComponent*>(components[i]))
				componentIndex = i;
		}
		if (componentIndex > 0)
			components.erase(components.begin() + componentIndex);
	}

private:
	vector<Component*> components;
};


//TODO: rewrite as singleton
/*
Class which is used for entities creating/removing ONLY
It just stores all available entities and pull of free indexes for new entities
and assign right index for new entity
*/
class EntityManager {
public:
	Entity* create() {
		//TODO: change to smart pointer
		Entity* entity = new Entity();
		entities.push_back(entity);
		return entities.back();
	}

	void remove(Entity* entity) {
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}

	vector<Entity*> getEntities() {
		return entities;
	}

private:
	vector<Entity*> entities;
};
