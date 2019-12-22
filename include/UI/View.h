#pragma once
#include "Entity.h"

class View {
protected:
	std::shared_ptr<Entity> _entity;
public:
	bool setEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> getEntity();
	virtual ~View() = default;
};

class ViewFactory {
	virtual std::shared_ptr<View> createView() = 0;
};
