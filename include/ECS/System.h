#pragma once
#include "General.h"
#include "Entity.h"

/*
System implements the logic of game. In update method some user defined manipulations
with registered entities should be produced. Every system contains ComponentMask which is 
calculated in class Constructor. Reference to world is needed to get access to related to entity
components through ComponentManager class.
*/
class System {
public:
	// Called every game update
	virtual void update(shared_ptr<EntityManager> entityManager) {};

};

class SystemManager {
public:
	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> create() {
		shared_ptr<ConcreteSystem> system = get<ConcreteSystem>();
		if (system == nullptr) {
			system = make_shared<ConcreteSystem>();
			systems.push_back(system);
			return system;
		}
		return nullptr;
	}

	//get existing system or create new one
	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> get() {
		for (auto system : systems) {
			shared_ptr<ConcreteSystem> foundSystem = dynamic_pointer_cast<ConcreteSystem>(system);
			if (foundSystem)
				return foundSystem;
		}
		return nullptr;
	}

private:
	vector<shared_ptr<System> > systems;	  //References to all systems to notify about changes
};