#pragma once
#include "Entity.h"

class View {
protected:
	std::shared_ptr<Entity> _entity;
	std::string _viewName;
public:
	View();
	virtual bool initialize() = 0;
	bool setEntity(std::shared_ptr<Entity> entity);
	
	std::string getName();
	std::shared_ptr<Entity> getEntity();
	virtual ~View() = default;
};

class ViewFactory {
public:
	virtual std::shared_ptr<View> createView(std::string name = "View", std::shared_ptr<View> parent = nullptr) = 0;
};
