#include "Grid.h"

Grid::Grid(std::string name) {
	_viewName = name;
}

bool Grid::initialize() {
	for (int row = 0; row < std::get<0>(_dim); row++) {
		for (int col = 0; col < std::get<1>(_dim); col++) {
			//use default position (0,0)
			auto view = _views[col + row * std::get<1>(_dim)];
			view->initialize();
		}
	}
	return false;
}

bool Grid::setDim(std::tuple<int, int> dim) {
	_dim = dim;
	return false;
}

bool Grid::setSize(std::tuple<int, int> size) {
	_size = size;
	return false;
}