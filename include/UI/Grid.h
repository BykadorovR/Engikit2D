#pragma once
#include "View.h"

class Grid : public View {
private:
	std::tuple<int, int> _dim;
	std::tuple<int, int> _size;
public:
	Grid(std::string name);
	bool initialize();

	bool setDim(std::tuple<int, int> dim);
	bool setSize(std::tuple<int, int> size);
};

class GridFactory {
public:
	template <class Type>
	std::shared_ptr<Grid> createGrid(std::tuple<int, int> dim, std::string name = "Grid", std::shared_ptr<View> parent = nullptr) {
		std::shared_ptr<Grid> grid = std::make_shared<Grid>(name);
		grid->setParent(parent);
		grid->setDim(dim);
		int rows = std::get<0>(dim), cols = std::get<1>(dim);
		for (int i = 0; i < rows * cols; i++) {
			grid->addView(std::make_shared<Type>());
		}

		return grid;
	}
};