#include "Back.h"
#include "State.h"
#include "GraphicComponents.h"

Back::Back(std::string name = "Back") {
	_viewName = name;
}

bool Back::initialize() {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize({ std::get<0>(resolution) / 2, std::get<1>(resolution) / 2 }, { 100, 100 }, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextureComponent>()->initialize(bufferManager);
	return false;
}

bool Back::setPosition(std::tuple<float, float> position) {
	_entity->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(position));
	_entity->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(position));
	return false;
}

bool Back::setSize(std::tuple<float, float> size) {
	_entity->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(size));
	_entity->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(size));
	return false;
}

bool Back::setColorMask(std::vector<float> mask) {
	_entity->getComponent<TextureComponent>()->setColorMask(mask);
	return false;
}

bool Back::setColorAddition(std::vector<float> addition) {
	_entity->getComponent<TextureComponent>()->setColorAddition(addition);
	return false;
}

bool Back::setTexture(int textureID) {
	_entity->getComponent<TextureComponent>()->setTexture(textureID);
	return false;
}

std::tuple<float, float> Back::getPosition() {
	return _entity->getComponent<ObjectComponent>()->getPosition();
}

std::tuple<float, float> Back::getSize() {
	return _entity->getComponent<ObjectComponent>()->getSize();
}

BackFactory::BackFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
}

std::shared_ptr<View> BackFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<Back> back = std::make_shared<Back>(name);
	back->setEntity(_activeScene->createEntity());
	back->getEntity()->createComponent<ObjectComponent>();
	back->getEntity()->createComponent<TextureComponent>();
	return back;
}