#pragma once
#include "General.h"
#include "Entity.h"
#include "Component.h"
#include "World.h"

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

	// Add a reference to the parent world
	void registerWorld(World* world) {
		parentWorld = world;
	}

	// This entity fits our current requirements
	//should be called by world during add component call if bitmasks are equal
	void registerEntity(EntityID entity) {
		registeredEntities.push_back(entity);
	}

	// This entity has stopped fitting our current requirements
	void deRegisterEntity(EntityID entity) {
		for (auto it = registeredEntities.begin(); it != registeredEntities.end(); ++it) {
			EntityID e = *it;
			if (e == entity) {
				registeredEntities.erase(it);
				return;
			}
		}
	}

	ComponentMask getMask() {
		return systemSignature;
	}
protected:
	// Specifies which components our system cares about — its size should = the number of different components
	ComponentMask systemSignature;
	// These entities fit the systemSignature and should be iterated upon for any functionality
	std::vector<EntityID> registeredEntities;
	// Reference to our parent world
	World* parentWorld;
};