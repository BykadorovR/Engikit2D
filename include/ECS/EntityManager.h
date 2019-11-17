#pragma once
#include <vector>
#include <memory>

#include "Entity.h"

class EntityManager {
public:
	EntityManager();
	std::shared_ptr<Entity> create(std::shared_ptr<Entity> _entity = nullptr);
	bool unregisterEntity(const std::shared_ptr<Entity>& entity);
	bool registerEntity(std::shared_ptr<Entity> entity);
	std::vector<std::shared_ptr<Entity> > getEntities();

private:
	std::vector<std::shared_ptr<Entity> > _entities;
};