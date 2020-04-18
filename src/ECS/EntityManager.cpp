#include "EntityManager.h"
#include <array>

std::shared_ptr<Entity> EntityManager::create() {
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	_entities.push_back({ entity, EntityState::ENTITY_REGISTERED });
	int index = _entities.size() == 1 ? 0 : _entities.size() - 2;
	entity->setIndex(std::get<0>(_entities[index])->getIndex() + 1);
	return std::get<0>(_entities.back());
}

bool EntityManager::removeEntity(const std::shared_ptr<Entity>& entity) {
	for (auto local : _entities) {
		if (std::get<0>(local) == entity) {
			_entities.erase(std::remove(_entities.begin(), _entities.end(), local), _entities.end());
			entity->setIndex(-1);
			return true;
		}
	}
	return false;
}

bool EntityManager::addEntity(std::shared_ptr<Entity> entity) {
	for (auto local : _entities) {
		if (std::get<0>(local) == entity) {
			return false;
		}
	}
	_entities.push_back({ entity, EntityState::ENTITY_REGISTERED });
	int index = _entities.size() == 1 ? 0 : _entities.size() - 2;
	entity->setIndex(std::get<0>(_entities[index])->getIndex() + 1);
	return true;
}

bool EntityManager::unregisterEntity(std::shared_ptr<Entity> entity) {
	for (auto& local : _entities) {
		if (std::get<0>(local) == entity) {
			std::get<1>(local) = EntityState::ENTITY_UNREGISTERED;
			return true;
		}
	}
	return false;
}

bool EntityManager::registerEntity(std::shared_ptr<Entity> entity) {
	for (auto& local : _entities) {
		if (std::get<0>(local) == entity) {
			std::get<1>(local) = EntityState::ENTITY_REGISTERED;
			return true;
		}
	}
	return false;
}

std::vector<std::tuple<std::shared_ptr<Entity>, EntityState > > EntityManager::getEntities() {
	return _entities;
}