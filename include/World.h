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
		entityManager = new EntityManager();
	}

	Entity* createEntity() {
		Entity* entity = entityManager->create();
		return entity;
	}

	void registerSystem(System* system) {
		systems.push_back(system);
		system->setEntityManager(entityManager);
	}

	//get existing system or create new one
	template <class ConcreteSystem>
	ConcreteSystem getSystem() {
		for (auto system : systems) {
			ConcreteSystem* concrete = dynamic_cast<ConcreteSystem*>(system);
			if (concrete)
				return *concrete;
		}
		//TODO: change to smart pointer
		ConcreteSystem* system = new ConcreteSystem();
		systems.push_back(system);
		return *system;
	}

	//TODO: UPDATE
	void update(int dt) {
		for (auto system : systems) {
			system->update(dt);
		}
	}


private:
	//change to smart pointer
	EntityManager* entityManager; //Reference to entityManager (to be able delete entity ID)
	vector<System*> systems;	  //References to all systems to notify about changes
};