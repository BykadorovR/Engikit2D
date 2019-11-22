#include "EntityManager.h"
#include <array>

std::shared_ptr<Entity> EntityManager::create(std::shared_ptr<Entity> _entity) {
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	_entities.push_back(entity);
	int index = _entities.size() == 1 ? 0 : _entities.size() - 2;
	entity->setIndex(_entities[index]->getIndex() + 1);
	return _entities.back();
}

bool EntityManager::unregisterEntity(const std::shared_ptr<Entity>& entity) {
	for (auto local : _entities) {
		if (local == entity) {
			_entities.erase(std::remove(_entities.begin(), _entities.end(), entity), _entities.end());
			entity->setIndex(-1);
			return true;
		}
	}
	return false;
}

bool EntityManager::registerEntity(std::shared_ptr<Entity> entity) {
	for (auto local : _entities) {
		if (local == entity) {
			return false;
		}
	}
	_entities.push_back(entity);
	int index = _entities.size() == 1 ? 0 : _entities.size() - 2;
	entity->setIndex(_entities[index]->getIndex() + 1);
	return true;
}

std::vector<std::shared_ptr<Entity> > EntityManager::getEntities() {
	return _entities;
}