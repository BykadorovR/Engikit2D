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

	std::shared_ptr<Entity> createEntity(std::string name = "Entity");
	bool removeEntity(int entityID);
	bool removeEntity(const std::shared_ptr<Entity>& entity);
	bool addEntity(std::shared_ptr<Entity> entity);

	std::shared_ptr<EntityManager> getEntityManager();
	std::shared_ptr<SystemManager> getSystemManager();
	std::string getName();
};