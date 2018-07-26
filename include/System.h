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
	// Initialize the System — This happens *before* the game starts but *after* the world has been registered.
	virtual void init() {};

	// Called every game update
	virtual void update(int dt) {};

	// Called every game render
	virtual void render() {};

	vector<shared_ptr<Entity> > getEntities() {
		return entityManager->getEntities();
	}
	
	//Set entity pool current system will work with
	void setEntityManager(shared_ptr<EntityManager> _entityManager) {
		entityManager = _entityManager;
	}
private:
	//Reference to EntityManager to be able obtain all related Entities
	shared_ptr<EntityManager> entityManager;
};

class SystemManager {
public:
	vector<shared_ptr<System> > getSystems() {
		return systems;
	}

	template <class ConcreteSystem>
	shared_ptr<ConcreteSystem> create(shared_ptr<EntityManager> entityManager) {
		shared_ptr<ConcreteSystem> system = get<ConcreteSystem>();
		if (system == nullptr) {
			system = make_shared<ConcreteSystem>();
			dynamic_pointer_cast<System>(system)->setEntityManager(entityManager);
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
	template <class ConcreteSystem>
	bool unregister(const shared_ptr<ConcreteSystem>& system) {
		for (auto local : systems) {
			if (local == system) {
				systems.erase(std::remove(systems.begin(), systems.end(), system), systems.end());
				return true;
			}
		}
		return false;
	}

private:
	vector<shared_ptr<System> > systems;	  //References to all systems to notify about changes
};