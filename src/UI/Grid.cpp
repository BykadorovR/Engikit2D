#include "GraphicComponents.h"
#include "Grid.h"

Grid::Grid(std::string name) {
	_viewName = name;
}

bool Grid::setPosition(std::tuple<float, float> position) {
	_position = position;

	float startX = std::get<0>(position);
	float startY = std::get<1>(position);

	for (int row = 0; row < std::get<0>(_dim); row++) {
		for (int col = 0; col < std::get<1>(_dim); col++) {
			//use default position (0,0)
			auto view = _views[col + row * std::get<1>(_dim)];
			auto objectComponent = view->getEntity()->getComponent<ObjectComponent>();
			objectComponent->setMember("positionX", startX);
			objectComponent->setMember("positionY", startY);
			startX += *std::get<0>(objectComponent->getMemberFloat("sizeX"));
		}
		startX = std::get<0>(position);
		auto startRowView = _views[row * std::get<1>(_dim)];
		startY += *std::get<0>(startRowView->getEntity()->getComponent<ObjectComponent>()->getMemberFloat("sizeY"));
	}

	return false;
}

bool Grid::initialize() {
	int startX = 0;
	int startY = 0;
	for (auto& view : _views) {
		view->initialize();
	}
	setSize({ 100, 20 });
	setPosition({ startX, startY });

	return false;
}

bool Grid::setDim(std::tuple<float, float> dim) {
	_dim = dim;
	return false;
}

bool Grid::setSize(std::tuple<float, float> size) {
	_size = size;
	for (int row = 0; row < std::get<0>(_dim); row++) {
		for (int col = 0; col < std::get<1>(_dim); col++) {
			//use default position (0,0)
			auto view = _views[col + row * std::get<1>(_dim)];
			auto objectComponent = view->getEntity()->getComponent<ObjectComponent>();
			objectComponent->setMember("sizeX", std::get<0>(size));
			objectComponent->setMember("sizeY", std::get<1>(size));
		}
	}
	return false;
}

GridFactory::GridFactory(std::shared_ptr<Scene> activeScene) {
	_scene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<Grid> GridFactory::createGridBack(std::tuple<int, int> dim, std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<Grid> grid = std::make_shared<Grid>(name);
	grid->setParent(parent);
	grid->setDim(dim);
	int rows = std::get<0>(dim), cols = std::get<1>(dim);
	for (int i = 0; i < rows * cols; i++) {
		grid->addView(_backFactory->createView());
	}

	return grid;
}