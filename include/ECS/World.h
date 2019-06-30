#pragma once
#include "General.h"
#include "System.h"
#include "Entity.h"

/*
Class connector which contains all needed information for convinient way of
ECS parts manipulating: list of all component managers,  systems, entity masks and entity manager.
It is used for creating entities, registration systems and some internal communications.
*/
class World {
public:
	World() {
		entityManager = make_shared<EntityManager>();
		systemManager = make_shared<SystemManager>();
	}

	shared_ptr<Entity> createEntity() {
		shared_ptr<Entity> entity = entityManager->create();
		return entity;
	}

	bool unregisterEntity(int entityID) {
		auto entity = entityManager->getEntities()[entityID];
		return entityManager->unregisterEntity(entity);
	}

	bool unregisterEntity(const shared_ptr<Entity>& entity) {
		return entityManager->unregisterEntity(entity);
	}

	bool registerEntity(shared_ptr<Entity> entity) {
		return entityManager->registerEntity(entity);
	}

	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> createSystem() {
		return systemManager->create<ConcreteSystem>();
	}

	//get existing system or create new one
	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> getSystem() {
		return systemManager->get<ConcreteSystem>();
	}

	shared_ptr<EntityManager> getEntityManager() {
		return entityManager;
	}

	shared_ptr<SystemManager> getSystemManager() {
		return systemManager;
	}

private:
	shared_ptr<EntityManager> entityManager; //Reference to entityManager (to be able delete entity ID)
	shared_ptr<SystemManager> systemManager;
};