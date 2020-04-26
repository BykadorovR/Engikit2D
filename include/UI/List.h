#pragma once
#include "View.h"
#include "Scene.h"
#include "GlyphsLoader.h"

class List : public View {
public:
	List(std::string name);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<std::string> items);
};

class ListFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
public:
	ListFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "List");
};