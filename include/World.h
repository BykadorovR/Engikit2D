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
	//TODO: change component in case of existence or throw error
	//we store copies of components because we don't want to change the initial objects passed by user
	template<class ConcreteComponent>
	void addComponent(EntityID e, ConcreteComponent c) {
		ComponentManager<ConcreteComponent>* manager = getComponentManager<ConcreteComponent>(e);
		//TODO: check unique of component
		manager->add(e, c);

		//update mask and register in system if needed
		entityMask[e].addComponent<ConcreteComponent>();
		for (auto system : systems) {
			if (entityMask[e].checkMask(system->getMask())) {
				system->registerEntity(e);
			}
		}
	}

	template<class ConcreteComponent>
	void removeComponent(EntityID e) {
		ComponentManager<ConcreteComponent>* manager = getComponentManager<ConcreteComponent>(e);
		manager->destroy(e);

		//update mask and deregister in system if needed
		for (auto system : systems) {
			if (entityMask[entity].checkMask(system->getMask())) {
				system.deRegisterEntity(e);
			}
		}
		entityMask[e].removeComponent<ConcreteComponent>();
	}

	//we don't want to return pointer because user will be able to change internal state of component by manipulating the pointer
	template<class ConcreteComponent>
	ConcreteComponent& getComponent(EntityID e) {
		ComponentManager<ConcreteComponent>* manager = getComponentManager<ConcreteComponent>(e);
		return manager->get(e);
	}

	EntityHandle createEntity() {
		EntityHandle handle(entityManager->create(), this);
		return handle;
	}

	void addSystem(System* system) {
		system->registerWorld(this);
		for (auto entity : entityManager->entities) {
			if (entityMask[entity].checkMask(system->getMask())) {
				system->registerEntity(entity);
			}
		}

		systems.push_back(system);
	}

	//get existing system or create new one
	template <class SpecificSystem>
	SpecificSystem getSystem() {
		for (auto system : systems) {
			SpecificSystem* concrete = dynamic_cast<SpecificSystem*>(system);
			if (concrete)
				return *concrete;
		}
		//TODO: change to smart pointer
		SpecificSystem* system = new SpecificSystem();
		systems.push_back(system);
		return *system;
	}

	// Destroy an entity and all its components
	void destroyEntity(EntityID e) {

	}

	//TODO: UPDATE
	void update(int dt) {
		for (auto system : systems) {
			system->update(dt);
		}
	}


private:
	template<class ConcreteComponent>
	ComponentManager<ConcreteComponent>* getComponentManager(EntityID e) {
		int familyID = getComponentFamily<ConcreteComponent>();
		if (!componentManager[familyID]) {
			componentManager[familyID] = (BaseComponentManager*) new ComponentManager<ConcreteComponent>();
		}

		ComponentManager<ConcreteComponent>* manager = (ComponentManager<ConcreteComponent>*) componentManager[familyID];
		return manager;
	}

	//change to smart pointer
	std::array<BaseComponentManager*, MAX_COMPONENT_MANAGERS> componentManager = { 0 }; //References to all componentManagers
	EntityManager* entityManager; //Reference to entityManager (to be able delete entity ID)
	vector<System*> systems;	  //References to all systems to notify about changes
	map<EntityID, ComponentMask> entityMask;
};