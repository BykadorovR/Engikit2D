#pragma once
#include "View.h"
#include "Scene.h"
#include "Back.h"

class Grid : public View {
private:
	std::tuple<float, float> _dim;
	std::tuple<float, float> _size;
	std::tuple<float, float> _position;
public:
	Grid(std::string name);
	bool initialize();
	bool setPosition(std::tuple<float, float> position);
	bool setDim(std::tuple<float, float> dim);
	bool setSize(std::tuple<float, float> size);
};

class GridFactory {
private:
	std::shared_ptr<Scene> _scene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	GridFactory(std::shared_ptr<Scene> activeScene);
	//TODO: add create to existing factories
	std::shared_ptr<Grid> createGridBack(std::tuple<int, int> dim, std::string name = "Grid", std::shared_ptr<View> parent = nullptr);
};