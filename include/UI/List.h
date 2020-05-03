#pragma once
#include "View.h"
#include "Scene.h"
#include "GlyphsLoader.h"
#include "Back.h"

class List : public View {
public:
	List(std::string name);
	bool initialize();
	bool addItem(std::string item);
	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<float, float> size);
};

class ListFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<ViewFactory> _itemFactory;
public:
	ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory);
	std::shared_ptr<View> createView(std::string name = "List", std::shared_ptr<View> parent = nullptr);
};