#pragma once
#include "EntityManager.h"
#include "SystemManager.h"
#include "BufferManager.h"

class Scene {
private:
	std::shared_ptr<EntityManager> _entityManager; //Reference to entityManager (to be able delete entity ID)
	std::shared_ptr<SystemManager> _systemManager;
	std::shared_ptr<BufferManager> _bufferManager;
	std::string _name;
public:
	Scene(std::string name);

	std::shared_ptr<Entity> createEntity();
	bool unregisterEntity(int entityID);
	bool unregisterEntity(const std::shared_ptr<Entity>& entity);
	bool registerEntity(std::shared_ptr<Entity> entity);

	std::shared_ptr<EntityManager> getEntityManager();
	std::shared_ptr<SystemManager> getSystemManager();
	std::string getName();
	bool isEntityAttached(std::shared_ptr<Entity> entity);
};