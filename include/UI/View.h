#pragma once
#include "Entity.h"
#include "Operation.h"

class View {
protected:
	//name (ID)
	std::string _viewName;
	//parent
	std::shared_ptr<View> _parent;
	//composite view
	std::vector<std::shared_ptr<View> > _views;
	//owned entity
	std::shared_ptr<Entity> _entity;
public:
	View();
	std::string getName();
	std::vector<std::shared_ptr<View> > getViews();
	std::shared_ptr<Entity> getEntity();
	bool setEntity(std::shared_ptr<Entity> entity);

	virtual ~View() = default;
};

class ViewFactory {
	virtual std::shared_ptr<View> createView(std::string name = "View") = 0;
};
