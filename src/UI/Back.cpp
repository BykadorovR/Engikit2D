#include "Back.h"
#include "GraphicComponents.h"

Back::Back(std::string name = "Back") {
	_viewName = name;
}

bool Back::initialize(std::tuple<float, float> position, std::tuple<float, float> size, int textureID, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, shader);
	_entity->getComponent<TextureComponent>()->initialize(textureID, bufferManager);
	return false;
}

bool Back::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> color, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, shader);
	_entity->getComponent<TextureComponent>()->initialize(bufferManager);
	_entity->getComponent<TextureComponent>()->setColorMask({ 0, 0, 0, 0 });
	_entity->getComponent<TextureComponent>()->setColorAddition(color);
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

std::shared_ptr<View> BackFactory::createView(std::string name) {
	std::shared_ptr<Back> back = std::make_shared<Back>(name);
	back->setEntity(_activeScene->createEntity());
	back->getEntity()->createComponent<ObjectComponent>();
	back->getEntity()->createComponent<TextureComponent>();
	return back;
}