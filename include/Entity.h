#pragma once
#include "General.h"
#include "World.h"

class World;

/*
Just ID of entity
*/
class EntityID {
public:
	EntityID(unsigned int _id) : id(_id) {

	}
	EntityID() {

	}
	unsigned int getID() {
		return id;
	}

	operator int() {
		return id;
	}

	bool operator<(const EntityID& right) const {
		return id < right.id;
	}
private:
	unsigned int id;
};

//TODO: Some kind of Proxy?
/*
Class needed to simplify access to entities and work with them
So instead of using world with add(e,c), component and entity can be linked
with each other through entity, e.g. e.add(c)
*/
struct EntityHandle {
	EntityHandle(EntityID _entity, World* _world) : entity(_entity), world(_world) {
	}

	template<typename ConcreteComponent>
	void addComponent(ConcreteComponent _component) {
		world->addComponent(entity, _component);
	}

	template<typename ConcreteComponent>
	ConcreteComponent getComponent() {
		return world->getComponent<ConcreteComponent>(entity);
	}

	template<typename ConcreteComponent>
	void removeComponent() {
		world->removeComponent(entity);
	}

	EntityID getEntity() {
		return entity;
	}

private:
	EntityID entity;
	World* world;
};

//TODO: rewrite as singleton
/*
Class which is used for entities creating/removing ONLY
It just stores all available entities and pull of free indexes for new entities
and assign right index for new entity
*/
class EntityManager {
public:
	EntityID create() {
		if (free_list.size() != 0) {
			EntityID last_element = free_list.back();
			entities.push_back(last_element);
			free_list.pop_back();
		}
		else {
			entities.push_back(entities.size());
		}
		return entities.back();
	}

	void remove(EntityID e) {
		entities.erase(std::remove(entities.begin(), entities.end(), e.getID()), entities.end());
		free_list.push_back(e.getID());
	}

	vector<EntityID> entities;
	vector<EntityID> free_list;
};
