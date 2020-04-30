#pragma once
#include "View.h"
#include "Scene.h"
#include "GlyphsLoader.h"
#include "Back.h"

class List : public View {
	std::vector<std::shared_ptr<View> > _views;
	int _page;
public:
	List(std::string name);
	bool initialize();
	std::shared_ptr<Back> getBack();
	std::vector<std::shared_ptr<View> > getViews();
	bool setBack(std::shared_ptr<View> back);
	bool addItem(std::string item);
	bool addView(std::shared_ptr<View> view);
	bool setPosition(std::tuple<float, float> postion);
	bool setSize(std::tuple<float, float> size);
};

class ListFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
	std::shared_ptr<ViewFactory> _itemFactory;
public:
	ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory);
	std::shared_ptr<View> createView(std::string name = "List", std::shared_ptr<View> parent = nullptr);
};