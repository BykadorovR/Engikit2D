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

	void setEntityManager(shared_ptr<EntityManager> manager) {
		entityManager = manager;
	}
protected:
	//Reference to EntityManager to be able obtain all related Entities
	shared_ptr<EntityManager> entityManager;
};