#include "Back.h"
#include "GraphicComponents.h"

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

BackFactory::BackFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
}

std::shared_ptr<View> BackFactory::createView() {
	std::shared_ptr<Back> button = std::make_shared<Back>();
	button->setEntity(_activeScene->createEntity());
	button->getEntity()->createComponent<ObjectComponent>();
	button->getEntity()->createComponent<TextureComponent>();
	return button;
}