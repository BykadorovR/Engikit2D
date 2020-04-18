#pragma once
#include <vector>
#include <memory>

#include "Entity.h"

enum EntityState {
	ENTITY_REGISTERED = 0,
	ENTITY_UNREGISTERED
};

class EntityManager {
public:
	std::shared_ptr<Entity> create();
	bool unregisterEntity(std::shared_ptr<Entity> entity);
	bool registerEntity(std::shared_ptr<Entity> entity);
	bool removeEntity(const std::shared_ptr<Entity>& entity);
	bool addEntity(std::shared_ptr<Entity> entity);
	std::vector<std::tuple<std::shared_ptr<Entity>, EntityState > > getEntities();

private:
	std::vector<std::tuple<std::shared_ptr<Entity>, EntityState > > _entities;
};