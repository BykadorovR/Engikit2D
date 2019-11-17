#pragma once
#include "EntityManager.h"
#include "SystemManager.h"

class Scene {
private:
	std::shared_ptr<EntityManager> _entityManager; //Reference to entityManager (to be able delete entity ID)
	std::shared_ptr<SystemManager> _systemManager;
	std::shared_ptr<BufferManager> _bufferManager;
	std::string _name;
public:
	Scene();

	std::shared_ptr<Entity> createEntity();
	bool unregisterEntity(int entityID);
	bool unregisterEntity(const std::shared_ptr<Entity>& entity);
	bool registerEntity(std::shared_ptr<Entity> entity);

	std::shared_ptr<EntityManager> getEntityManager();
	std::shared_ptr<SystemManager> getSystemManager();
	bool isEntityAttached(std::shared_ptr<Entity> entity);
};