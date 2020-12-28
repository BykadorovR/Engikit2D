#include "Scene.h"

Scene::Scene(std::string name) {
	_entityManager = std::make_shared<EntityManager>();
	_systemManager = std::make_shared<SystemManager>();
	_name = name;
}

std::shared_ptr<Entity> Scene::createEntity(std::string name) {
	std::shared_ptr<Entity> entity = _entityManager->create(name);
	return entity;
}

bool Scene::removeEntity(int entityID) {
	auto entity = _entityManager->getEntities()[entityID];
	return _entityManager->removeEntity(std::get<0>(entity));
}

bool Scene::removeEntity(const std::shared_ptr<Entity>& entity) {
	return _entityManager->removeEntity(entity);
}

bool Scene::addEntity(std::shared_ptr<Entity> entity) {
	return _entityManager->addEntity(entity);
}

std::shared_ptr<EntityManager> Scene::getEntityManager() {
	return _entityManager;
}

std::shared_ptr<SystemManager> Scene::getSystemManager() {
	return _systemManager;
}

std::string Scene::getName() {
	return _name;
}