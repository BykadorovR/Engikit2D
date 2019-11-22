#include "Scene.h"

Scene::Scene(std::string name) {
	_entityManager = std::make_shared<EntityManager>();
	_systemManager = std::make_shared<SystemManager>();
	_bufferManager = std::make_shared<BufferManager>();
	_name = name;
}

std::shared_ptr<Entity> Scene::createEntity() {
	std::shared_ptr<Entity> entity = _entityManager->create();
	return entity;
}

bool Scene::unregisterEntity(int entityID) {
	auto entity = _entityManager->getEntities()[entityID];
	return _entityManager->unregisterEntity(entity);
}

bool Scene::unregisterEntity(const std::shared_ptr<Entity>& entity) {
	return _entityManager->unregisterEntity(entity);
}

bool Scene::registerEntity(std::shared_ptr<Entity> entity) {
	return _entityManager->registerEntity(entity);
}

std::shared_ptr<EntityManager> Scene::getEntityManager() {
	return _entityManager;
}

std::shared_ptr<SystemManager> Scene::getSystemManager() {
	return _systemManager;
}

bool Scene::isEntityAttached(std::shared_ptr<Entity> entity) {
	for (auto &attached : _entityManager->getEntities()) {
		if (entity == attached)
			return true;
	}
	return false;
}

std::string Scene::getName() {
	return _name;
}