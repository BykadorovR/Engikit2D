#pragma once
#include "View.h"
#include "Scene.h"
#include "GlyphsLoader.h"
#include "Back.h"

class List : public View {
public:
	List(std::string name);
	bool initialize();
	bool addItem(std::string text);
	bool addItem(std::vector<std::string> text);
	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<float, float> size);
	bool setEditable(bool editable);
	bool clear();
};

class ListFactory : public ViewFactory {
protected:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<ViewFactory> _itemFactory;
public:
	ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory);
	std::shared_ptr<View> createView(std::string name = "List", std::shared_ptr<View> parent = nullptr);
	//TODO: very tricky logic with create grid. In case of list it's list of grids of views. In case of any other views it's grid of views.
	std::shared_ptr<View> createGrid(std::tuple<int, int> dim, std::string name = "List", std::shared_ptr<View> parent = nullptr);
};