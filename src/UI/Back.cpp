#include "Back.h"
#include "State.h"
#include "GraphicComponents.h"
#include "UtilityComponents.h"

Back::Back(std::string name) {
	_viewName = name;
}

bool Back::initialize() {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize({ std::get<0>(currentResolution) / 2, std::get<1>(currentResolution) / 2 }, { 100, 100 }, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextureComponent>()->initialize(bufferManager);
	_entity->getComponent<GroupComponent>()->initialize("Default");
	//default "texture" is solid fill
	//TODO: make some default store for all default parameters
	setColorMask({ 0, 0, 0, 0 });
	setColorAddition({ 0.5f, 0, 0.5f, 1 });
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
	back->setEntity(_activeScene->createEntity("Back"));
	back->getEntity()->createComponent<ObjectComponent>();
	back->getEntity()->createComponent<TextureComponent>();
	back->getEntity()->createComponent<GroupComponent>();
	back->setParent(parent);
	return back;
}

std::shared_ptr<View> BackFactory::createGrid(std::tuple<int, int> dim, std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<Grid> grid = std::make_shared<Grid>(name);
	grid->setParent(parent);
	grid->setDim(dim);
	int cols = std::get<0>(dim), rows = std::get<1>(dim);
	for (int i = 0; i < rows * cols; i++) {
		grid->addView(createView());
	}

	return grid;
}