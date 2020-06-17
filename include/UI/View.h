#pragma once
#include "Entity.h"

class View {
protected:
	std::shared_ptr<Entity> _entity;
	std::string _viewName;
	std::vector<std::shared_ptr<View> > _views;
	std::shared_ptr<View> _parent;
public:
	View();
	virtual bool initialize() = 0;
	std::shared_ptr<Entity> getEntity();
	bool setEntity(std::shared_ptr<Entity> entity);
	std::string getName();
	bool addView(std::shared_ptr<View> view);
	std::vector<std::shared_ptr<View> > getViews();
	std::vector<std::shared_ptr<Entity> > getEntities();
	bool setParent(std::shared_ptr<View> parent);
	std::shared_ptr<View> getParent();
	virtual ~View() = default;
};

class ViewFactory {
public:
	virtual std::shared_ptr<View> createView(std::string name = "View", std::shared_ptr<View> parent = nullptr) = 0;
	virtual std::shared_ptr<View> createGrid(std::tuple<int, int> dim, std::string name = "List", std::shared_ptr<View> parent = nullptr) {
		return nullptr;
	};
};
