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
	}

	shared_ptr<Entity> createEntity() {
		shared_ptr<Entity> entity = entityManager->create();
		return entity;
	}

	void registerSystem(shared_ptr<System> system) {
		systems.push_back(system);
		system->setEntityManager(entityManager);
	}

	//get existing system or create new one
	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> getSystem() {
		for (auto system : systems) {
			auto concrete = dynamic_pointer_cast<ConcreteSystem>(system);
			if (concrete)
				return concrete;
		}

		shared_ptr<ConcreteSystem> system = make_shared<ConcreteSystem>();
		systems.push_back(system);
		return system;
	}

	void update(int dt) {
		for (auto system : systems) {
			system->update(dt);
		}
	}


private:
	shared_ptr<EntityManager> entityManager; //Reference to entityManager (to be able delete entity ID)
	vector<shared_ptr<System> > systems;	  //References to all systems to notify about changes
};