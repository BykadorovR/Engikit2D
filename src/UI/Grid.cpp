#include "GraphicComponents.h"
#include "Grid.h"

Grid::Grid(std::string name) {
	_viewName = name;
}

bool Grid::setPosition(std::tuple<float, float> position) {
	_position = position;

	float startX = std::get<0>(position);
	float startY = std::get<1>(position);

	for (int row = 0; row < std::get<1>(_dim); row++) {
		for (int col = 0; col < std::get<0>(_dim); col++) {
			//use default position (0,0)
			auto view = _views[col + row * std::get<0>(_dim)];
			auto objectComponent = view->getEntity()->getComponent<ObjectComponent>();
			objectComponent->setMember("positionX", startX);
			objectComponent->setMember("positionY", startY);
			startX += *std::get<0>(objectComponent->getMemberFloat("sizeX")) + 1;
		}
		startX = std::get<0>(position);
		auto startRowView = _views[row * std::get<0>(_dim)];
		startY += *std::get<0>(startRowView->getEntity()->getComponent<ObjectComponent>()->getMemberFloat("sizeY")) + 1;
	}

	return false;
}

bool Grid::initialize() {
	//TODO: make default init more smart so if view was inited before initialize() we won't override it with default parameters here
	int startX = 0;
	int startY = 0;
	std::vector<std::tuple<float, float> > size;
	for (auto& view : _views) {
		view->initialize();
		//TODO: move to default settings
		size.push_back({ 100, 20 });
	}

	setSize(size);
	setPosition({ startX, startY });

	return false;
}

bool Grid::setDim(std::tuple<float, float> dim) {
	_dim = dim;
	return false;
}

bool Grid::setSize(std::vector<std::tuple<float, float> > size) {
	_size = size;
	for (int row = 0; row < std::get<1>(_dim); row++) {
		for (int col = 0; col < std::get<0>(_dim); col++) {
			//use default position (0,0)
			auto view = _views[col + row * std::get<0>(_dim)];
			auto objectComponent = view->getEntity()->getComponent<ObjectComponent>();
			objectComponent->setMember("sizeX", std::get<0>(size[col]));
			objectComponent->setMember("sizeY", std::get<1>(size[col]));
		}
	}
	//redraw with new size
	setPosition(_position);
	return false;
}

std::tuple<float, float> Grid::getSize() {
	std::tuple<float, float> size = { 0, 0 };
	for (int col = 0; col < std::get<0>(_dim); col++) {
		auto view = _views[col];
		auto objectComponent = view->getEntity()->getComponent<ObjectComponent>();
		std::get<0>(size) += *std::get<0>(objectComponent->getMemberFloat("sizeX"));
		std::get<1>(size) = *std::get<0>(objectComponent->getMemberFloat("sizeY"));
	}
	std::get<1>(size) *= std::get<1>(_dim);
	return size;
}